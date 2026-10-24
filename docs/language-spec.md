# Kite language specification (draft)

## 1. Design philosophy

Kite is a small, explicit, expression-friendly teaching language whose syntax is regular enough
for a hand-written parser. Programs should make types and control flow visible while keeping the
compiler pipeline easy to inspect. Kite favors deterministic behavior, readable diagnostics, and
small standard-library primitives over implicit conversions or large runtime features. The working
name is **Kite** and source files use the `.kite` extension.

## 2. Lexical grammar

Source is UTF-8 text. The lexer treats bytes outside ASCII as string content but accepts only ASCII
letters, digits, and underscore in identifiers. Whitespace is space, tab, carriage return, or line
feed and is ignored except that it separates tokens. Statements end with `;`; newlines do not end
statements. Line comments begin with `//` and end at a line feed. Block comments begin with `/*`
and end with the first `*/`; block comments are not nested.

Identifiers match `[A-Za-z_][A-Za-z0-9_]*`. Reserved words are:

`fn let if else while for return true false unit int float bool string and or not`.

Integer literals are non-empty decimal digits and use signed 64-bit range. Float literals are
decimal digits, a dot, and decimal digits; floats are SHOULD. Strings use double quotes and support
`\\`, `\"`, `\n`, `\r`, and `\t`. Booleans are `true` and `false`. `unit` is both the unit type
name and unit literal. There is no null literal.

Operators and delimiters are:

`+ - * / % = == != < <= > >= |>` and `(` `)` `{` `}` `[` `]` `,` `:` `;` `->`.

The lexer emits an EOF token. An unterminated string or block comment is a lexical error and the
lexer resumes at the next line or EOF after recording the diagnostic.

## 3. EBNF grammar

```ebnf
program          = { top_level_item }, EOF ;
top_level_item   = function_decl | statement ;
function_decl    = "fn", identifier, "(", [ parameters ], ")", "->", type, block ;
parameters       = parameter, { ",", parameter } ;
parameter        = identifier, ":", type ;

statement        = let_statement | if_statement | while_statement | for_statement
                 | return_statement | block | expression_statement ;
let_statement    = "let", identifier, [ ":", type ], "=", expression, ";" ;
if_statement     = "if", "(", expression, ")", block, [ "else", (if_statement | block) ] ;
while_statement  = "while", "(", expression, ")", block ;
for_statement    = "for", "(", [ for_initializer ], ";", [ expression ], ";",
                   [ for_update ], ")", block ;
for_initializer  = "let", identifier, [ ":", type ], "=", expression
                 | assignment_expression ;
for_update       = assignment_expression ;
return_statement = "return", [ expression ], ";" ;
block            = "{", { statement }, "}" ;
expression_statement = expression, ";" ;

expression       = assignment_expression ;
assignment_expression = logic_or, [ "=", assignment_expression ] ;
logic_or         = logic_and, { "or", logic_and } ;
logic_and        = equality, { "and", equality } ;
equality         = comparison, { ( "==" | "!=" ), comparison } ;
comparison       = term, { ( "<" | "<=" | ">" | ">=" ), term } ;
term             = factor, { ( "+" | "-" | "|>" ), factor } ;
factor           = unary, { ( "*" | "/" | "%" ), unary } ;
unary            = ( "-" | "not" ), unary | postfix ;
postfix          = primary, { call_suffix | index_suffix } ;
call_suffix      = "(", [ arguments ], ")" ;
index_suffix     = "[", expression, "]" ;
arguments        = expression, { ",", expression } ;
primary          = integer | float | string | "true" | "false" | "unit"
                 | identifier | "(", expression, ")" | array_literal ;
array_literal    = "[", [ expression, { ",", expression } ], "]" ;

type             = primitive_type, { "[", "]" } ;
primitive_type   = "int" | "float" | "bool" | "string" | "unit" ;
identifier       = ASCII letter or underscore, { ASCII letter, digit, or underscore } ;
```

The grammar is parsed with recursive descent for declarations/statements and Pratt binding powers
for expressions. The `|>` operator is represented by a dedicated `PipeExpr`; its right operand must
be a function name, not an arbitrary expression. `x |> f` means `f(x)`.

## 4. Operator table

Rows are ordered from low to high precedence and are directly usable as Pratt binding powers.

| Binding power | Operator | Associativity | Arity | Operand types | Result |
|---:|---|---|---:|---|---|
| 1 | `=` | right | 2 | assignable target, matching value | `unit` |
| 2 | `|>` | left | 2 | value, function whose first parameter accepts value | function return type |
| 3 | `or` | left | 2 | `bool`, `bool` | `bool` |
| 4 | `and` | left | 2 | `bool`, `bool` | `bool` |
| 5 | `==`, `!=` | left | 2 | same non-error type | `bool` |
| 6 | `<`, `<=`, `>`, `>=` | left | 2 | two `int` or two `float` values | `bool` |
| 7 | `+`, `-` | left | 2 | two `int`, two `float`, or two `string` for `+` | matching numeric type or `string` |
| 8 | `*`, `/`, `%` | left | 2 | two `int` or two `float` for `*` and `/` | matching numeric type |
| 9 | unary `-`, `not` | right | 1 | numeric or `bool` respectively | matching type |
| 10 | call `(...)`, index `[...]` | left | postfix | callable or array/string | return type or element/`string` |

## 5. Type system

MUST types are `int`, `bool`, `string`, and `unit`. `float` and arrays are SHOULD. Array syntax is
`int[]`, `string[]`, or another primitive followed by `[]`. There are no user-defined types in the
MUST language.

Function parameters and return types require explicit annotations. A local `let` may omit its type
annotation; the initializer type becomes its type. No implicit conversions exist. If floats are
implemented, an explicit `int -> float` conversion is not a builtin conversion; `to_float` must be
used if the team adds it. `to_string` is an explicit conversion.

Core typing rules:

- `+ - * / %` require matching `int` operands; `+ - * /` also accept matching `float` operands.
- `string + string` is concatenation.
- Comparisons require matching numeric operands and produce `bool`.
- Equality requires equal operand types and produces `bool`.
- `and`, `or`, and `not` require `bool`.
- `if`, `while`, and `for` conditions require `bool`.
- Assignment requires a declared mutable target and an equal value type.
- A function call requires exact arity and assignable parameter types.
- A function body must return the declared type on every path unless the type is `unit`.
- A `return;` statement has type `unit`.
- An expression statement discards its value and has type `unit`.

Type errors are reported with codes `T001`–`T008` as defined in `docs/error-and-diagnostics.md`.

## 6. Scoping and binding

Kite uses lexical block scope. Function declarations are global and may be called before their
source declaration. Local variables are visible after their declaration and are not visible in
their initializer. A nested block may shadow a name from an outer block; redeclaring a name in the
same scope is `S001`. All `let` bindings and parameters are mutable. Global `let` bindings are
stored in the module global area; block-local bindings use stack-frame local slots.

## 7. Semantics

Expressions evaluate left to right. `and` and `or` short-circuit and evaluate the right operand
only when needed. There is no general truthiness conversion: only `bool` is accepted in conditions.
Integers are checked signed 64-bit values; overflow is a runtime error `R002`. Division or modulo
by zero is runtime error `R001`. Strings are immutable byte sequences with value equality and `+`
concatenation. Function calls evaluate arguments left to right, push a call frame, bind parameters
in slot order, and return one value. Recursion is supported until the configured VM call-stack
limit. The compiler creates a synthetic `__main` function from top-level statements and the VM
starts there.

## 8. Built-ins

| Built-in | Signature | Behavior |
|---|---|---|
| `print` | `print(value) -> unit` | Writes an int, float, bool, string, or unit plus newline to stdout. |
| `input` | `input() -> string` | Reads one line from stdin without the newline. |
| `len` | `len(string) -> int` and `len(T[]) -> int` | Returns byte length or array element count. |
| `to_string` | `to_string(int/bool/float) -> string` | Converts without surrounding quotes. |
| `to_int` | `to_int(string) -> int` | Parses a base-10 signed integer or raises `R005`. |

## 9. Runtime errors

Runtime errors are written to stderr with a code, source span when available, and message. Exit code
3 means a runtime error. `R001` is division/modulo by zero, `R002` integer overflow, `R003` array or
string index out of range, `R004` call-stack overflow, `R005` invalid `to_int`, and `R006` malformed
bytecode. Usage errors exit 2; compile errors exit 1; successful execution exits 0.

## 10. Compile-time errors

`L001` illegal character, `L002` unterminated comment, `L003` unterminated string, `L004` invalid
escape, `P001` unexpected token, `P002` expected token, `P003` invalid assignment target, `S001`
duplicate declaration, `S002` unresolved name, `S003` invalid forward reference, `T001` type
mismatch, `T002` non-boolean condition, `T003` non-callable target, `T004` wrong argument count,
`T005` argument type mismatch, `T006` return type mismatch, `T007` invalid operator operands,
and `T008` missing return.

## 11. Feature tiers

| Feature | Tier |
|---|---|
| Variables and mutable `let` | MUST |
| Integer arithmetic | MUST |
| Boolean comparison and logic | MUST |
| `if/else` | MUST |
| `while` | MUST |
| Functions | MUST |
| Recursion | MUST |
| Strings | MUST |
| Unit/void | MUST |
| C-style `for` | SHOULD |
| Floats | SHOULD |
| Arrays/lists | SHOULD |
| Pipe operator `|>` | SHOULD |
| VM `--trace` flag | MUST |
| Structs/records | STRETCH |
| Closures | FUTURE |
| Modules/imports | FUTURE |
| Type inference beyond local `let` | FUTURE |
| Constant folding/dead-code removal | STRETCH |
| REPL | STRETCH |
| Full debugger | FUTURE |

## 12. Distinctive-feature candidates

| Candidate | Syntax and semantics | Cost | Risk | Tier |
|---|---|---|---|---|
| Pipe operator | `5 |> double |> print` desugars left-to-right to `print(double(5))`; RHS is a function name. | M | Low; requires function lookup and one AST node. | SHOULD |
| Expression-oriented blocks | `let result = if (ok) { 1 } else { 2 };` returns the final expression. | M | Changes statement grammar and return typing. | STRETCH |
| Pattern-style `match` | `match value { 0 => print("zero"); _ => print("other"); }`. | M/L | Adds patterns, exhaustiveness, and new diagnostics. | STRETCH |

Only the pipe operator is selected for the planned distinctive feature. It is explicitly SHOULD,
not MUST; it is cut before any MUST work if schedule pressure appears.

## 13. Complete example programs

Each output line is terminated by `print`.

### Walking skeleton — `examples/hello.kite`

```kite
print(1 + 2);
```

Expected output:

```text
3
```

### Hello world — `examples/hello-world.kite`

```kite
print("Hello, Kite!");
```

Expected output:

```text
Hello, Kite!
```

### Arithmetic — `examples/arithmetic.kite`

```kite
let answer: int = 6 * 7 + 1;
print(answer);
```

Expected output: `43`.

### Variables and scope — `examples/scope.kite`

```kite
let x = 1;
{
  let x = 2;
  print(x);
}
print(x);
```

Expected output:

```text
2
1
```

### Conditionals — `examples/conditionals.kite`

```kite
let score = 7;
if (score >= 5 and true) {
  print("pass");
} else {
  print("fail");
}
```

Expected output: `pass`.

### Loops — `examples/loops.kite`

```kite
let i = 0;
while (i < 3) {
  print(i);
  i = i + 1;
}
```

Expected output:

```text
0
1
2
```

### Factorial and Fibonacci — `examples/recursion.kite`

```kite
fn fact(n: int) -> int {
  if (n <= 1) { return 1; }
  return n * fact(n - 1);
}

fn fib(n: int) -> int {
  if (n < 2) { return n; }
  return fib(n - 1) + fib(n - 2);
}

print(fact(5));
print(fib(6));
```

Expected output:

```text
120
8
```

### Multiple parameters — `examples/functions.kite`

```kite
fn add(a: int, b: int) -> int {
  return a + b;
}

print(add(2, 3));
```

Expected output: `5`.

### Strings — `examples/strings.kite`

```kite
let name = "Kite";
print("Hello, " + name);
print(len(name));
```

Expected output:

```text
Hello, Kite
4
```

### Pipe operator — `examples/pipe.kite`

```kite
fn double(n: int) -> int {
  return n * 2;
}

5 |> double |> print;
```

Expected output: `10`.

### SHOULD arrays and for — `examples/arrays-for.kite`

```kite
let values: int[] = [1, 2, 3];
let total = 0;
for (let i = 0; i < len(values); i = i + 1) {
  total = total + values[i];
}
print(total);
```

Expected output: `6`.

### SHOULD float — `examples/float.kite`

```kite
let value: float = 1.5 + 2.0;
print(value);
```

Expected output: `3.5`.

### Compile failure — `tests/errors/type-mismatch.kite`

```kite
let x: int = true;
```

Expected exit code: `1`.
Expected diagnostic fragment:

```text
1:14: T001 type mismatch: expected int, found bool
```

### Runtime failure — `tests/errors/division-by-zero.kite`

```kite
print(10 / 0);
```

Expected exit code: `3` and diagnostic fragment:

```text
R001 division by zero
```

## Open Questions

1. Confirm whether the real rubric adds requirements not represented by the generic traceability table.
2. Confirm whether `main() -> unit` must be source-visible instead of synthetic.
3. Confirm whether float formatting should use fixed precision or the host's shortest round-trip form.
4. Confirm whether array mutation is required; the current SHOULD design permits `values[i] = x`.

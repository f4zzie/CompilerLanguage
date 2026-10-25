# Kite code walkthrough

This guide explains the implemented compiler in presentation order and is grounded in the current
source tree. It can be used as the student presentation and oral-exam checklist.

## End-to-end flow

`kite run file.kite` is orchestrated by `src/driver/main.cpp`. The driver reads bytes, lexes them,
parses an AST, checks names and types, lowers the valid AST to bytecode, and executes the module in
the VM. A failure in lexing, parsing, or semantic analysis prints diagnostics and returns exit code
1; a VM failure returns exit code 3.

## Lexer — `src/lexer/lexer.cpp`

The lexer owns a cursor, byte offset, one-based line/column counters, and a token vector. It uses
longest-match checks for operators, skips whitespace/comments, decodes strings, parses integers and
floats, maps reserved words, and always emits EOF. It reports `L001`–`L004` through `Diagnostics`.
The important public function is `Lexer::scan(file, source, diagnostics)`.

Why this design: a cursor is easy to debug, requires no generated tooling, and preserves exact source
spans for diagnostics. The binary-input fuzz test specifically protects the cursor from confusing a
real NUL byte with end-of-input.

## Parser — `src/parser/parser.cpp`

Recursive descent handles declarations, statements, blocks, types, and recovery. Pratt parsing in
`parse_expression` handles precedence and associativity. `Parser::parse` returns a non-null `Program`
even when recoverable errors exist. Synchronization advances at semicolons, braces, EOF, and statement
keywords so one malformed statement does not hide the next one.

AST ownership is `std::unique_ptr`; nodes carry IDs and spans. `src/parser/ast_printer.cpp` produces
the canonical AST snapshot used by `tests/golden/hello.ast`.

## Semantic analysis — `src/sema/sema.cpp`

The analyzer predeclares functions to permit recursion, creates lexical symbol scopes, infers local
`let` types, validates operators and conditions, checks calls and returns, and records inferred types
on expressions. `src/sema/symbol.cpp` implements lexical lookup and same-scope duplicate rejection.
Errors use the documented `S001`–`S003` and `T001`–`T008` codes.

Why this design: explicit function signatures make call errors useful, while local inference keeps
small programs readable without adding a general inference engine.

## Code generation — `src/codegen/code_generator.cpp`

The emitter creates a function table, global slots, constants, and linear instructions. It patches
branch targets after emitting blocks. Locals are frame slots; top-level bindings use globals. Boolean
short-circuit operations are lowered to conditional jumps. Arrays use `MAKE_ARRAY` and `INDEX`.
`src/codegen/bytecode.cpp` provides stable opcode names and disassembly.

## VM — `src/runtime/vm.cpp`

The VM uses a `std::variant` `Value`, a value stack, a global vector, and a frame vector. Each frame
stores function index, instruction pointer, return instruction pointer, and local values. The dispatch
loop validates operands before indexing, executes arithmetic/comparison/control-flow operations,
invokes builtins, and reports runtime errors. `--trace` writes instruction and stack state to stderr.

The runtime supports integers, floats, booleans, strings, unit, and shared array values. Overflow,
division by zero, invalid indexes, malformed bytecode, and call-stack overflow have explicit codes.

## Diagnostics — `src/diagnostics/diagnostic.cpp`

`Diagnostics` collects errors up to a configurable cap, adds one suppression note, and sorts by file,
line, column, then code. `render_diagnostic` locates the source line and draws a caret/underline.
Diagnostics are kept separate from stage logic so lexer, parser, sema, and the driver share one output
format.

## Driver — `src/driver/cli.cpp` and `src/driver/main.cpp`

The hand-written CLI accepts `run`, `build`, `check`, `tokens`, `ast`, and `disasm`, plus `--trace`,
`--max-errors`, `--color`, and `-o`. The driver keeps stdout for program/dump output and stderr for
diagnostics/trace. `src/driver/pipeline.cpp` remains the reusable library orchestration entry point.

## Real trace: `print(1 + 2);`

Source:

```kite
print(1 + 2);
```

Token dump from `tests/golden/hello.kite`:

```text
1:1 identifier `print`
1:6 ( `(`
1:7 integer `1`
1:9 + `+`
1:11 integer `2`
1:12 ) `)`
1:13 ; `;`
2:1 eof ``
```

AST dump:

```text
Program#0 [tests/golden/hello.kite:1:1-2:1]
  ExprStmt#6 [tests/golden/hello.kite:1:1-1:14]
    CallExpr#2 callee=print [tests/golden/hello.kite:1:1-1:13]
      BinaryExpr#5 op=+ [tests/golden/hello.kite:1:7-1:12]
        LiteralExpr#3 kind=integer text="1" [tests/golden/hello.kite:1:7-1:8]
        LiteralExpr#4 kind=integer text="2" [tests/golden/hello.kite:1:11-1:12]
```

Disassembly:

```text
0	CONSTANT	0
1	CONSTANT	1
2	ADD	0
3	CALL	1
4	POP	0
5	HALT	0
```

Execution pushes 1, pushes 2, replaces them with 3, calls builtin `print`, discards the returned
unit, and halts. The observed stdout is exactly `3\n`.

## Oral-exam questions

**Why use a Pratt parser?** It keeps operator precedence in one binding-power table and makes new
operators local to token, precedence, AST, sema, codegen, and VM changes.

**Where is recursion enabled?** Functions are predeclared in `SemanticAnalyzer::analyze`, then CALL
creates a new frame in `Vm::run`.

**How are diagnostics deterministic?** The collector sorts by source file, line, column, and code;
dump printers do not iterate unordered maps for user-visible ordering.

**Why does `STORE_LOCAL` leave the value on the stack?** It preserves expression-statement and
assignment stack accounting so the bytecode emitter can explicitly discard values with `POP`.

**How is short-circuiting implemented?** The emitter creates a conditional jump before emitting the
right operand and supplies the boolean result for the skipped branch.

**What is the main limitation?** The current compiler is a teaching implementation: bytecode is
in-memory, `build -o` writes a disassembly text artifact rather than a binary module, and the real
grading rubric/other operating systems were not available for verification.

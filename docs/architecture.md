# Kite compiler architecture

## 1. Pipeline

```mermaid
flowchart LR
  S[Source text] --> L[Lexer]
  L --> T[Token stream]
  T --> P[Recursive descent + Pratt parser]
  P --> A[AST]
  A --> Q[Semantic analyzer]
  Q --> TA[Annotated AST]
  TA --> C[Bytecode generator]
  C --> B[Bytecode module]
  B --> V[Stack VM]
  V --> O[stdout/stderr]
```

The MUST pipeline has no separate optimization IR. `src/ir` contains an optional future linear IR
contract so an optimization pass can be added without changing the source language or VM.

## 2. Shared data contracts

### Source spans

```cpp
struct SourceLocation {
  std::string file;
  std::size_t line;
  std::size_t column;
  std::size_t offset;
};
struct Span { SourceLocation start; SourceLocation end; };
```

Offsets are byte offsets into the source string. Lines and columns are one-based. The end location
is exclusive. Every token, AST node, instruction, and diagnostic carries a span; synthetic objects
use file `<synthetic>`.

### Tokens

`TokenKind` in `src/lexer/token.hpp` has entries for EOF, identifiers, integer/float/string literals,
all reserved words, all operators, and all delimiters in `docs/language-spec.md`. A `Token` contains
`kind`, original `lexeme`, parsed numeric values when applicable, and `Span`.

### Diagnostics

```cpp
enum class DiagnosticSeverity { Note, Warning, Error };
struct DiagnosticNote { std::string message; Span span; };
struct Diagnostic {
  DiagnosticSeverity severity;
  std::string code;
  std::string message;
  Span primary_span;
  std::vector<DiagnosticNote> notes;
  std::string hint;
};
class Diagnostics {
  void add(Diagnostic);
  void error(std::string code, std::string message, Span);
  bool has_errors() const;
  void sort_deterministically();
};
```

The renderer prints `file:line:column`, code, message, the source line, and a caret/underline. It
does not emit ANSI color unless a future `--color=always` option requests it.

### AST hierarchy

The AST uses `std::unique_ptr` ownership and these abstract bases: `AstNode`, `Expr`, `Stmt`, and
`Decl`. Expressions are `LiteralExpr`, `NameExpr`, `UnaryExpr`, `BinaryExpr`, `CallExpr`,
`IndexExpr`, `ArrayExpr`, and `PipeExpr`. Statements are `ExprStmt`, `LetStmt`, `AssignStmt`, `BlockStmt`,
`IfStmt`, `WhileStmt`, `ForStmt`, and `ReturnStmt`. The declaration node is `FunctionDecl`.
`Program` owns top-level declarations and statements.

Every node has a monotonically assigned `NodeId` and `Span`. The semantic analyzer annotates each
expression's `inferred_type`, resolves names to symbols, and records function signatures. The parser
does not perform type checking or name lookup.

### Types and symbols

```cpp
enum class TypeKind { Int, Float, Bool, String, Unit, Array, Error };
struct Type { TypeKind kind; std::shared_ptr<Type> element; };
struct Symbol { std::string name; Type type; bool is_function; Span declared_at; };
class Scope { bool insert(Symbol); const Symbol* lookup(const std::string&) const; };
class SymbolTable { void push_scope(); void pop_scope(); bool insert(Symbol); const Symbol* lookup(...); };
```

Scopes are lexical. Insertion rejects duplicate names in one scope. Lookup searches the current
scope and parents. Function declarations are predeclared before body checking to permit recursion
and forward references.

## 3. Stage contracts

### 3.1 Source manager and diagnostics

**Responsibility:** retain source text by file name, create spans, collect diagnostics, and render
source-aware messages. **Non-responsibility:** it never decides whether syntax or types are valid.

**API:**

```cpp
void SourceManager::add(std::string file, std::string source);
std::string_view SourceManager::source_for(std::string_view file) const;
std::string render_diagnostic(const Diagnostic&, std::string_view source);
```

**Invariants:** all spans refer to the source file named by their start location; diagnostics are
bounded by `max_errors`; sorting is stable by file, line, column, then code. Test with a one-line
and multi-line source and verify caret width in `tests/unit/diagnostics_test.cpp`.

**Estimate:** 150–220 lines, 4–6 hours.

### 3.2 Lexer

**Responsibility:** convert source bytes into tokens, decode literals, and report lexical errors.
**Non-responsibility:** it does not build AST nodes or resolve identifiers.

**API:**

```cpp
std::vector<Token> Lexer::scan(std::string_view file,
                               std::string_view source,
                               Diagnostics& diagnostics);
```

The implementation is a cursor with line/column tracking. It recognizes longest-match operators
first (`->`, `==`, `!=`, `<=`, `>=`), skips comments, and emits EOF. Invalid characters consume one
byte and emit `L001`; unterminated strings/comments emit `L003`/`L002`. Tests use source strings and
golden token dumps under `tests/golden/*.tokens`.

**Invariant:** tokens are contiguous, ordered by source offset, and end with exactly one EOF token.
**Estimate:** 250–350 lines, 6–8 hours.

### 3.3 Parser

**Responsibility:** build an AST from the token vector using recursive descent and Pratt expressions.
**Non-responsibility:** it does not perform name or type checks.

**API:**

```cpp
std::unique_ptr<Program> Parser::parse(const std::vector<Token>&, Diagnostics&);
```

The parser keeps a current token index, uses binding powers from `parser/precedence.hpp`, and
creates a node span from first child start to last child end. Recovery synchronizes after `;`, `}`,
EOF, and top-level declaration keywords. Unexpected tokens produce `P001`; missing expected tokens
produce `P002`; invalid assignment targets produce `P003`.

**Invariant:** a returned program is non-null even when recoverable errors exist; it contains no
partially initialized child pointers. Tests compare AST dumps in `tests/golden/*.ast`.
**Estimate:** 500–700 lines, 12–16 hours.

### 3.4 Semantic analyzer

**Responsibility:** create scopes, predeclare functions, resolve names, infer local `let` types,
check expressions/statements, and annotate the AST. **Non-responsibility:** it does not emit bytecode.

**API:**

```cpp
SemanticResult SemanticAnalyzer::analyze(Program&, Diagnostics&);
```

The analyzer performs a global function-signature pass, then checks top-level statements and
function bodies. It uses explicit tables for builtin signatures. It collects independent errors
until the diagnostics cap and substitutes `Error` types to avoid cascades.

**Invariant:** code generation runs only when there are no error diagnostics; every expression has
an inferred type or `Error`; all names used by code generation resolve to a symbol or are rejected.
Tests use `tests/errors/*.kite` and sema unit fixtures.
**Estimate:** 450–650 lines, 12–16 hours.

### 3.5 Optional IR

**Responsibility:** provide an optional linear instruction representation for later constant folding
and dead-code removal. **Non-responsibility:** it is not required by the first backend.

**API:** `IrFunction` owns a vector of `IrInstruction`; an optional lowering pass accepts annotated
AST and returns `std::vector<IrFunction>`. It must preserve source spans and evaluation order.

**Invariant:** no optimization may change observable output or diagnostic locations. This stage is
STRETCH and is cut before any MUST feature.
**Estimate:** 250–400 lines, 8–12 hours.

### 3.6 Bytecode and code generation

**Responsibility:** lower the annotated AST into deterministic `BytecodeModule` data. **Non-
responsibility:** it does not re-check types or execute code.

**API:**

```cpp
BytecodeModule CodeGenerator::generate(const Program&, Diagnostics&);
std::string disassemble(const BytecodeModule&);
```

The generator builds a constants pool, reserves jump operands, emits function metadata, and patches
jumps after blocks are emitted. Locals use frame-relative slots; globals use a module global table.
Every emitted instruction retains the AST source span.

**Invariant:** every call target exists in the function table; jump targets are instruction indices;
stack height is balanced at block and function boundaries. `tests/golden/*.disasm` verifies output.
**Estimate:** 500–750 lines, 14–18 hours.

### 3.7 VM/runtime

**Responsibility:** execute bytecode, manage operand stack and call frames, invoke builtins, and
report runtime failures. **Non-responsibility:** it does not parse source or repair malformed modules.

**API:**

```cpp
VmResult Vm::run(const BytecodeModule& module);
```

`Value` is a variant of `int64_t`, `double`, `bool`, `std::string`, and `UnitValue`; array storage
is added only with the SHOULD array feature. A frame contains function index, instruction pointer,
base stack index, and return instruction index. The dispatch loop increments the instruction pointer
after fetching. `--trace` prints instruction index, opcode, and stack values to stderr.

**Invariant:** the VM never indexes outside its value stack; runtime failures contain an error code
and source span; calls and returns restore the previous frame exactly. Tests feed handcrafted modules
and use end-to-end programs under `tests/programs/`.
**Estimate:** 550–800 lines, 16–20 hours.

### 3.8 Driver/CLI

**Responsibility:** parse arguments, read a source file, run selected pipeline stages, render dumps,
and map failures to exit codes. **Non-responsibility:** it does not contain language semantics.

**Commands:** `run`, `build`, `check`, `tokens`, `ast`, `disasm`.

**Flags:** `--trace`, `--max-errors N`, and `--color=auto|always|never`. `stdout` is program output
or requested dump output; diagnostics and trace go to `stderr`. Exit codes are 0 success, 1 compile
error, 2 usage error, 3 runtime error, and 4 internal error.

**Estimate:** 220–320 lines, 5–7 hours.

## 4. Bytecode instruction set

The stack column lists the stack from bottom to top before and after execution. `x` means a runtime
value, `a` an address/index, and `n` an immediate operand.

| Opcode | Operand | Before → after | Semantics | Producer |
|---|---|---|---|---|
| `CONSTANT` | pool index | `S → S, c` | Push constant pool value. | literals |
| `UNIT` | none | `S → S, unit` | Push unit. | unit literal/return |
| `ADD` | none | `S,a,b → S,a+b` | Numeric add or string concatenate. | `+` |
| `SUBTRACT` | none | `S,a,b → S,a-b` | Checked numeric subtraction. | `-` |
| `MULTIPLY` | none | `S,a,b → S,a*b` | Checked numeric multiplication. | `*` |
| `DIVIDE` | none | `S,a,b → S,a/b` | Reject zero divisor. | `/` |
| `MODULO` | none | `S,a,b → S,a%b` | Integer modulo; reject zero divisor. | `%` |
| `NEGATE` | none | `S,a → S,-a` | Checked numeric negation. | unary `-` |
| `NOT` | none | `S,b → S,!b` | Boolean negation. | `not` |
| `EQUAL` | none | `S,a,b → S,a==b` | Typed equality. | `==`, `!=` uses inversion |
| `LESS` | none | `S,a,b → S,a<b` | Numeric comparison. | `<`, `<=` uses inversion |
| `GREATER` | none | `S,a,b → S,a>b` | Numeric comparison. | `>`, `>=` uses inversion |
| `JUMP` | instruction index | `S → S` | Set instruction pointer. | branches/loops |
| `JUMP_IF_FALSE` | instruction index | `S,b → S` | Jump if false, otherwise continue. | conditions |
| `LOAD_LOCAL` | slot | `S → S,v` | Push frame-local value. | names |
| `STORE_LOCAL` | slot | `S,v → S,v` | Store without consuming expression value. | `let`/assignment |
| `LOAD_GLOBAL` | slot | `S → S,v` | Push module global. | global names |
| `STORE_GLOBAL` | slot | `S,v → S,v` | Store global. | global `let` |
| `CALL` | function index/arity | `S,args → S,result` | Invoke function or builtin. | calls/pipe |
| `RETURN` | none | `S,v → callerS,v` | Restore caller frame. | `return` |
| `POP` | none | `S,v → S` | Discard expression result. | expression statement |
| `HALT` | none | `S → S` | End synthetic `__main`. | module entry |

The emitter must produce at least one instruction for every AST construct in the MUST tier. Boolean
short-circuiting uses `JUMP_IF_FALSE` and explicit merge labels. `!=`, `<=`, and `>=` may be lowered
using existing comparison plus `NOT`.

## 5. Extensibility recipes

### Add an operator

1. Add a token kind and lexer spelling in `src/lexer/token.hpp` and `src/lexer/lexer.cpp`.
2. Add precedence and associativity in `src/parser/precedence.hpp`.
3. Add an AST enum member in `src/parser/ast.hpp`.
4. Add type rules and diagnostic tests in `src/sema/sema.cpp` and `tests/errors/`.
5. Add an opcode or lowering sequence in `src/codegen/bytecode.hpp` and `code_generator.cpp`.
6. Add VM behavior in `src/runtime/vm.cpp` and a positive/negative test.
7. Update the language spec, operator table, feature matrix, and disassembly golden file.

### Add a statement

1. Add an AST node and grammar production.
2. Parse it with a recovery synchronization point.
3. Add scope/type checks.
4. Emit bytecode and define stack effects.
5. Add AST, disassembly, positive, and negative tests.
6. Update examples and the rubric traceability row.

### Add a builtin

1. Add its signature to the sema builtin registry.
2. Add its implementation to `src/runtime/builtins.cpp`.
3. Define argument and runtime error behavior.
4. Add one positive and one invalid-input test.
5. Update the builtin table and CLI/demo documentation.

### Add a type

1. Extend `TypeKind`, type parsing, equality, and pretty-printing.
2. Add literal and operator rules.
3. Extend `Value` and its display function.
4. Define bytecode and VM storage semantics.
5. Add positive, mismatch, and runtime-boundary tests.
6. Update the feature tier and implementation estimate.

## 6. Performance and determinism targets

The compiler should process a 1,000-line program in under one second in Debug on the team laptop,
and `fib(30)` should finish in a few seconds in Release. No user-visible output may depend on
unordered-map iteration: dump output sorts names or uses insertion-order vectors. The same source,
compiler version, and flags must produce byte-for-byte identical dumps.

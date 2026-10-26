# Kite study guide: ten commands through the compiler

Run these commands from the repository root after configuring and building the Debug preset:

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
```

The outputs below are captured from the Linux Debug build used for the post-release verification on
October 26, 2026. File paths and source spans are therefore intentionally visible in the dumps.

## 1. Tokens: see the lexer output

```sh
./build/linux-debug/kite tokens examples/hello.kite
```

Output:

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

This proves that the lexer converts characters into ordered tokens, preserves source locations, and
adds exactly one EOF token. The token dump is implemented in `src/lexer/lexer.cpp` and
`src/lexer/token_dump.cpp`.

## 2. AST: see the parser's tree

```sh
./build/linux-debug/kite ast examples/hello.kite
```

Output:

```text
Program#0 [examples/hello.kite:1:1-2:1]
  ExprStmt#6 [examples/hello.kite:1:1-1:14]
    CallExpr#2 callee=print [examples/hello.kite:1:1-1:13]
      BinaryExpr#5 op=+ [examples/hello.kite:1:7-1:12]
        LiteralExpr#3 kind=integer text="1" [examples/hello.kite:1:7-1:8]
        LiteralExpr#4 kind=integer text="2" [examples/hello.kite:1:11-1:12]
```

This proves that recursive-descent parsing builds statements and Pratt parsing builds the nested
binary expression with the intended precedence. Node IDs and spans come from `src/parser/parser.cpp`;
the printable form is in `src/parser/ast_printer.cpp`.

## 3. Bytecode: see the backend result

```sh
./build/linux-debug/kite disasm examples/hello.kite
```

Output:

```text
0	CONSTANT	0
1	CONSTANT	1
2	ADD	0
3	CALL	1
4	POP	0
5	HALT	0
```

The backend has lowered `1 + 2`, the `print` call, and the statement discard into stack-machine
instructions. The opcode table is in `src/codegen/bytecode.hpp`; emission is in
`src/codegen/code_generator.cpp`.

## 4. VM trace: watch execution, not just output

```sh
./build/linux-debug/kite run examples/pipe.kite --trace
```

Program output on stdout:

```text
10
```

Trace on stderr:

```text
0 CONSTANT []
1 CALL [5]
5 LOAD_LOCAL []
6 CONSTANT [5]
7 MULTIPLY [5, 2]
8 RETURN [10]
2 CALL [10]
3 POP [unit]
4 HALT []
```

The trace shows instruction indices, opcodes, and the operand stack before each instruction. It
demonstrates that `5 |> double |> print` is ordinary bytecode execution through calls and returns.
The dispatch loop and trace formatting are in `src/runtime/vm.cpp`.

## 5. Compile-time error: read a caret diagnostic

```sh
./build/linux-debug/kite run tests/errors/type-mismatch.kite
```

Output on stderr; the command exits with status `1`:

```text
tests/errors/type-mismatch.kite:1:14: T001 type mismatch: expected int, found bool
let x: int = true;
             ^~~~
```

This proves that the lexer/parser/semantic-analysis path stops before code generation when a
declared type and initializer disagree. `src/sema/sema.cpp` creates the diagnostic and
`src/diagnostics/diagnostic.cpp` renders the source line and caret.

## 6. Runtime error: distinguish VM failure from compilation failure

```sh
./build/linux-debug/kite run tests/errors/division-by-zero.kite
```

Output on stderr; the command exits with status `3`:

```text
runtime[R001]: division by zero
```

The source is valid, so compilation succeeds and the VM reports the failure while executing the
divide instruction. Runtime error codes and the exit-code mapping are defined by `src/runtime/vm.cpp`
and `src/driver/main.cpp`.

## 7. Recursion: follow calls and returns

```sh
./build/linux-debug/kite run examples/recursion.kite
```

Output:

```text
120
8
```

The first line is `fact(5)` and the second is `fib(6)`. This proves that function metadata, call
frames, local slots, and return restoration work for nested recursive calls. The frame contract is
documented in `docs/architecture.md` and exercised by `tests/unit/recursion_test.cpp`.

## 8. Floats: use the implemented SHOULD feature

```sh
./build/linux-debug/kite run examples/float.kite
```

Output:

```text
3.5
```

The semantic analyzer accepts matching `float` operands, the code generator stores a `double`
constant, and the VM performs numeric arithmetic. Relevant code is in `src/sema/sema.cpp`,
`src/codegen/code_generator.cpp`, and `src/runtime/vm.cpp`.

## 9. Arrays and the C-style `for` loop

```sh
./build/linux-debug/kite run examples/arrays-for.kite
```

Output:

```text
6
```

The program constructs `[1, 2, 3]`, loops over its indices, reads each element, and accumulates the
sum. This demonstrates `ArrayExpr`, `MAKE_ARRAY`, `INDEX`, loop initialization/condition/update,
and lexical loop scope. See `src/parser/ast.hpp`, `src/codegen/code_generator.cpp`, and
`src/runtime/vm.cpp`.

## 10. The pipe operator: a compact language feature

```sh
./build/linux-debug/kite run examples/pipe.kite
```

Output:

```text
10
```

The pipe operator passes the value on its left as the single argument to the named function on its
right. The parser creates `PipeExpr`, semantic analysis checks the target signature, and code
generation emits an ordinary `CALL`. Follow the path through `src/parser/parser.cpp`, `src/sema/sema.cpp`,
and `src/codegen/code_generator.cpp`.

## How to use this guide in an oral exam

For every command, answer three questions: which stage produced the visible artifact, what invariant
the artifact demonstrates, and which test would fail if that stage regressed. That turns the output
from a memorized demo into evidence about the implementation.

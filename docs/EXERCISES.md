# Kite extension exercises

These exercises use the extension recipes in `docs/architecture.md`. Each answer names the first
test to write, the implementation files, and the verification command. The six exercises were each
implemented on a separate temporary `scratch/` branch during post-release preparation and then
discarded. Their smoke results were:

| Exercise | Scratch result |
|---|---|
| New builtin `is_zero` | `true` printed by a source program |
| New binary operator `^` | integer XOR `5 ^ 3` printed `6` |
| New keyword statement `say` | `hello from say` printed |
| New compile-time error `T009` | exit `1` with the empty-array diagnostic |
| New opcode `DUPLICATE` | targeted doctest `1/1` passed; `61` other cases skipped |
| Changed diagnostic wording | exit `1` with the revised T001 message |

The examples below are teaching exercises, not changes to the released language. A student should
make each change on a new branch and merge only after adding permanent tests and updating the spec.

## 1. Add a new builtin function: `is_zero`

### Goal

Make `is_zero(0)` return `true` and reject non-integer arguments.

### Write the test first

Add a positive corpus program before implementation:

```text
tests/programs/is-zero.src
tests/programs/is-zero.expected
tests/programs/is-zero.exitcode
```

Use:

```text
print(is_zero(0));
```

Expected output is `true`, with exit code `0`. Add a second invalid-input test that proves the runtime
error path is not silently accepting a string.

### Files to touch

- `src/sema/sema.cpp` — register the builtin signature and return type.
- `src/codegen/code_generator.cpp` — add the builtin to the function table.
- `src/runtime/vm.cpp` or a refactored builtin dispatch — implement the argument check and result.
- `tests/programs/` and `tests/errors/` — positive and invalid-input fixtures.
- `docs/language-spec.md` and the builtin documentation — public contract.

### Answer / verification

Follow the existing `print`, `len`, and `to_int` paths. The scratch implementation used the three
runtime/compiler files above and verified:

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/linux-debug/kite run tests/scratch_builtin.kite
```

Real output was:

```text
true
```

The permanent version should be checked with `./scripts/test-all` and the negative corpus runner.

## 2. Add a new binary operator: integer XOR `^`

### Goal

Make `5 ^ 3` produce `6`, with both operands statically typed as `int`.

### Write the test first

Start with a parser/sema/codegen test and a source fixture:

```text
tests/unit/xor_test.cpp
tests/programs/xor.src
tests/programs/xor.expected
tests/programs/xor.exitcode
```

The source fixture can be:

```text
print(5 ^ 3);
```

Also add a negative fixture for `true ^ false` so the type rule is tested.

### Files to touch

- `src/lexer/token.hpp` and `src/lexer/lexer.cpp` — token kind and spelling.
- `src/parser/precedence.hpp`, `src/parser/ast.hpp`, and `src/parser/parser.cpp` — precedence and AST enum.
- `src/parser/ast_printer.cpp` — deterministic AST spelling.
- `src/sema/sema.cpp` — require two integers and return `int`.
- `src/codegen/bytecode.hpp`, `bytecode.cpp`, and `code_generator.cpp` — opcode or lowering sequence.
- `src/runtime/vm.cpp` — integer XOR execution and malformed-bytecode handling.
- golden, positive, and negative tests plus the language specification.

### Answer / verification

The scratch implementation added `Caret`, `BitwiseXor`, and `BITWISE_XOR` through those layers. It
was verified with:

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/linux-debug/kite run tests/scratch_xor.kite
```

Real output was `6`. The permanent change must also update disassembly goldens and the operator table.

## 3. Add a new keyword statement: `say expression;`

### Goal

Add a statement-level spelling that evaluates an expression and prints it, lowering to the existing
`print` builtin.

### Write the test first

Create:

```text
tests/programs/say.src
tests/programs/say.expected
tests/programs/say.exitcode
tests/unit/parser_statement_test.cpp
```

The source should contain:

```text
say "hello from say";
```

The parser test should assert the new statement node and its child expression.

### Files to touch

- `src/lexer/token.hpp` and `src/lexer/lexer.cpp` — reserve `say`.
- `src/parser/ast.hpp`, `parser.hpp`, and `parser.cpp` — add and parse `SayStmt`.
- `src/parser/ast_printer.cpp` — print the new node.
- `src/sema/sema.cpp` — type-check its expression.
- `src/codegen/code_generator.cpp` — emit expression, `CALL print`, and `POP`.
- parser, positive, and malformed-statement tests; update the grammar/spec.

### Answer / verification

The scratch implementation was checked with the command above and printed:

```text
hello from say
```

The important design choice is reuse: `say` does not need a new VM opcode because it lowers to an
existing call contract.

## 4. Add a compile-time error code: `T009`

### Goal

Reject an unannotated empty array because its element type cannot be inferred.

### Write the test first

Create a complete negative fixture:

```text
tests/errors/empty-array.kite
tests/errors/empty-array.expected
tests/errors/empty-array.exitcode
tests/errors/empty-array.stderr
```

Source:

```text
let values = [];
```

The expected exit code is `1`; the expected message fragment is:

```text
T009 empty array needs an explicit element type
```

### Files to touch

- `src/sema/sema.cpp` — detect the condition and report `T009`.
- `docs/error-and-diagnostics.md` — add the code and message contract.
- `tests/errors/` and `docs/coverage-report.md` — fixture and inventory.

### Answer / verification

The scratch implementation produced this real diagnostic and exit status:

```text
tests/scratch_empty_array.kite:1:14: T009 empty array needs an explicit element type
let values = [];
             ^~
exit=1
```

The permanent test belongs in `run-errors.py` so future changes cannot accidentally remove coverage.

## 5. Add a new opcode: `DUPLICATE`

### Goal

Add an opcode that copies the value at the top of the operand stack. It should make `7 + 7` possible
from bytecode without emitting the constant twice.

### Write the test first

Add a handcrafted-module unit test:

```text
tests/unit/duplicate_opcode_test.cpp
```

The module should emit `CONSTANT 7`, `DUPLICATE`, `ADD`, `HALT` and assert the VM result is `14`.
This is the right level because the opcode is the behavior under test, not the parser.

### Files to touch

- `src/codegen/bytecode.hpp` and `bytecode.cpp` — enum member and disassembly name.
- `src/runtime/vm.cpp` — bounds check and stack copy.
- `src/codegen/code_generator.cpp` — use it from a real lowering if the language needs it.
- `tests/unit/duplicate_opcode_test.cpp`, and golden disassembly if source code emits it.
- `docs/architecture.md` — stack effect and producer.

### Answer / verification

The scratch version added `DUPLICATE`, built the full Debug target, and ran CTest:

```text
100% tests passed, 0 tests failed out of 7
```

The direct doctest runner then reported `58` passing cases. The released project deliberately does not
retain this teaching opcode because it is not required by the current language contract.

## 6. Change a diagnostic message safely

### Goal

Change the T001 wording from `expected int, found bool` to `expected int but found bool` without
changing the code, span, exit status, or caret rendering.

### Write the test first

Update or add a negative fixture that asserts the complete message fragment:

```text
tests/errors/type-mismatch.kite
tests/errors/type-mismatch.expected
tests/errors/type-mismatch.exitcode
tests/errors/type-mismatch.stderr
```

The test must keep exit code `1` and continue to assert the caret location.

### Files to touch

- `src/sema/sema.cpp` — message construction.
- `tests/errors/` — expected message files.
- `docs/error-and-diagnostics.md` and any report examples — user-facing wording.

### Answer / verification

The scratch branch produced:

```text
tests/scratch_diagnostic.kite:1:14: T001 type mismatch: expected int but found bool
let x: int = true;
             ^~~~
exit=1
```

Run the complete negative corpus after the change. Diagnostic wording is an external contract, so the
fixture update is part of the implementation, not a test workaround.

# Testing strategy

## Test pyramid

### Unit tests

Each stage has direct tests with no filesystem dependency where possible:

| Stage | Input | Assertion |
|---|---|---|
| Lexer | source string | ordered token kinds, lexemes, and spans |
| Parser | token vector | canonical AST dump |
| Semantic analyzer | AST | inferred types and diagnostic codes |
| Code generator | typed AST | canonical disassembly and stack-height checks |
| VM | handcrafted bytecode | output value, trace, or runtime error |
| Diagnostics | source plus span | exact line/column/caret rendering |

Unit tests live under `tests/unit/*_test.cpp` and use the vendored doctest header.

### Golden tests

Golden inputs use a shared basename:

```text
tests/golden/hello.kite
tests/golden/hello.tokens
tests/golden/hello.ast
tests/golden/hello.disasm
```

The regeneration command will be `scripts/update-golden.sh` or its PowerShell equivalent after T022.
Regeneration must be explicit, and every diff is reviewed as text. Dumps sort names and use stable
numeric IDs so output does not depend on hash-map iteration.

### End-to-end tests

Each program uses:

```text
tests/programs/NAME.src       Kite source
tests/programs/NAME.expected  exact stdout, including final newline
tests/programs/NAME.exitcode  optional decimal exit code; default 0
tests/programs/NAME.stdin     optional stdin fixture
```

The runner compiles the Debug binary, executes each program with a 2-second timeout, captures stdout
and stderr separately, compares exit code and expected stdout, and prints the first unified diff.
Runtime failures may use `NAME.stderr` for exact diagnostic text.

### Negative tests

Negative programs live under `tests/errors`. The scaffold accepts either `.kite` or `.src` source
files so course fixtures can be copied without renaming; the runner treats both as Kite source.
Each has an `.expected` diagnostic fragment and `.exitcode` containing `1` for compile failures or
`3` for runtime failures. The runner asserts code, one-based line, one-based column, message
fragment, and caret presence.

### Property/fuzz tests

The SHOULD fuzz target feeds arbitrary bytes to lexer and parser entry points and asserts that the
compiler neither crashes nor hangs for 100 ms per input. A deterministic corpus seed is committed;
libFuzzer integration is STRETCH and may be replaced by a bounded random-input executable.

## Regression policy

Every fixed bug starts with a failing test. The test is placed at the narrowest relevant layer and
one end-to-end case is added when the bug crosses a stage boundary. No test is deleted merely to make
the suite green.

## Coverage goals

The MUST target is 80% line coverage for lexer, parser, sema, codegen, and VM, plus one end-to-end
program for every feature. Coverage is measured with `gcovr` if available; otherwise the project
reports CTest counts and an explicit feature matrix. Coverage tools are not a runtime dependency.

## Feature-to-test matrix

| Feature | Positive test | Negative test |
|---|---|---|
| Integers/arithmetic | `programs/arithmetic.src` | `errors/overflow.src` |
| Booleans/logic | `programs/conditionals.src` | `errors/non-bool-condition.src` |
| Variables/scopes | `programs/scope.src` | `errors/duplicate-name.src` |
| `if/else` | `programs/conditionals.src` | `errors/non-bool-condition.src` |
| `while` | `programs/loops.src` | `errors/non-bool-condition.src` |
| Functions | `programs/functions.src` | `errors/wrong-arity.src` |
| Recursion | `programs/recursion.src` | `errors/missing-return.src` |
| Strings | `programs/strings.src` | `errors/bad-operator.src` |
| Unit | `programs/unit.src` | `errors/return-mismatch.src` |
| Builtins | `programs/builtins.src` | `errors/bad-to-int.src` |
| `for` SHOULD | `programs/arrays-for.src` | `errors/index-type.src` |
| Floats SHOULD | `programs/float.src` | `errors/float-int-mismatch.src` |
| Arrays SHOULD | `programs/arrays-for.src` | `errors/index-range.src` |
| Pipe SHOULD | `programs/pipe.src` | `errors/pipe-target.src` |
| Trace | `programs/trace.src` | `errors/trace-usage.src` |

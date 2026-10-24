# Kite report evidence checklist

- [x] Compiler: GCC 16.2.1; host: Linux x86_64, kernel 7.2.11-Arch1-1.
- [x] `cmake --preset linux-debug` configured successfully.
- [x] `cmake --build --preset linux-debug` completed with warnings as errors.
- [x] `ctest --test-dir build/linux-debug --output-on-failure`: 7/7 tests passed.
- [x] `kite tokens/ast/disasm examples/recursion.kite` captured in `/tmp/agent-tmp/recursion.{tokens,ast,disasm}` during verification.
- [x] `kite run tests/errors/type-mismatch.kite`: exit 1 with caret diagnostic.
- [x] `kite run tests/errors/division-by-zero.kite`: exit 3 with `runtime[R001]` diagnostic.
- [x] Positive corpus: 3 programs; negative corpus: 2 fixtures with exit-code/stderr assertions; golden: 3 snapshots; examples: 12.
- [x] SHOULD features implemented: floats, arrays, `for`, pipe operator, and trace output.
- [x] Release performance sample: `fib(30)` produced `832040` in 11.479 seconds on this host; the Debug 1,000-line timing was not recorded because the first attempted measurement used unavailable `/usr/bin/time` and the later combined process exceeded the command timeout.

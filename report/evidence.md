# Kite report evidence checklist

- [ ] Record compiler version and host operating system.
- [ ] Record `cmake --preset linux-debug` output.
- [ ] Record `cmake --build --preset linux-debug` output.
- [ ] Record CTest counts and the exact command used.
- [ ] Capture token, AST, and disassembly output for `examples/recursion.kite`.
- [ ] Capture a caret diagnostic from `tests/errors/type-mismatch.kite`.
- [ ] Capture a runtime diagnostic from `tests/errors/division-by-zero.kite`.
- [ ] Record positive/negative/golden test counts.
- [ ] Record any SHOULD features cut before release.

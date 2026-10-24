# Rubric traceability

No assignment rubric has been supplied yet. This generic matrix is the temporary baseline. Replace
or extend rows one-for-one when the official rubric arrives.

| Generic criterion | Planned deliverable | Verification | Demo step |
|---|---|---|---|
| Lexical analysis | `src/lexer`, `docs/language-spec.md` | lexer unit and token golden tests | run `kite tokens examples/hello.kite` |
| Parsing | `src/parser`, AST dump command | AST golden tests and parser recovery negatives | show `kite ast examples/recursion.kite` |
| Semantic analysis | `src/sema` | type/name negative corpus | run `tests/errors/type-mismatch.kite` |
| Code generation | `src/codegen` | disassembly golden tests | show `kite disasm examples/recursion.kite` |
| Runtime/execution | `src/runtime` | VM unit and positive end-to-end tests | run recursion example |
| Error handling | `src/diagnostics`, `docs/error-and-diagnostics.md` | exact code/location/caret tests | show compile and runtime failures |
| Testing | `docs/testing-strategy.md`, `tests/` | CTest and scripts/test-all | show test command and result |
| Documentation | README, spec, architecture, report plan | link/path review | navigate docs index |
| Distinctive feature | pipe SHOULD and `--trace` MUST | pipe positive test and trace snapshot | run `kite run examples/pipe.kite --trace` |
| Demonstration | `docs/demo-and-report-plan.md`, `examples/` | clean-machine rehearsal | follow demo script |
| Reproducibility | CMake presets, scripts, CI | clean configure/build/test | clone and run quick start |

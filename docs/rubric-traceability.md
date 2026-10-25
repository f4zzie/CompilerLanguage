# Rubric traceability

No assignment rubric has been supplied as of October 25, 2026. This generic matrix is therefore the
evidence-backed baseline; replace or extend rows one-for-one when the official rubric arrives.

| Generic criterion | Planned deliverable | Verification | Demo step |
|---|---|---|---|
| Lexical analysis | `src/lexer`, `docs/language-spec.md` | `tests/unit/lexer_test.cpp`, `tests/golden/hello.tokens`, CTest | run `kite tokens examples/hello.kite` |
| Parsing | `src/parser`, AST dump command | `tests/unit/parser_*.cpp`, `tests/golden/hello.ast`, CTest | show `kite ast examples/recursion.kite` |
| Semantic analysis | `src/sema` | `tests/unit/sema_test.cpp`, `tests/errors/`, `scripts/run-errors.py` | run `tests/errors/type-mismatch.kite` |
| Code generation | `src/codegen` | `tests/unit/codegen_test.cpp`, `tests/golden/hello.disasm`, CTest | show `kite disasm examples/recursion.kite` |
| Runtime/execution | `src/runtime` | `tests/unit/vm_test.cpp`, `tests/programs/`, `scripts/run-corpus.py` | run `examples/recursion.kite` |
| Error handling | `src/diagnostics`, `docs/error-and-diagnostics.md` | `tests/unit/diagnostics_test.cpp`, `tests/errors/`, exit-code checks | show compile and runtime failures |
| Testing | `docs/testing-strategy.md`, `tests/` | `ctest --test-dir build/linux-debug --output-on-failure`, `scripts/test-all` | show test command and result |
| Documentation | `README.md`, `docs/`, `report/` | `scripts/self-review.py`, package manifest check | navigate docs index |
| Distinctive feature | `|>` SHOULD and `--trace` MUST | `tests/unit/pipe_test.cpp`, `examples/pipe.expected`, CTest | run `kite run examples/pipe.kite --trace` |
| Demonstration | `docs/demo-and-report-plan.md`, `examples/` | `report/demo-smoke.sh`, clean archive rehearsal | follow demo script |
| Reproducibility | `CMakePresets.json`, `scripts/`, `.github/workflows/ci.yml` | `cmake --preset linux-debug`, `cmake --preset release`, `cmake --preset linux-asan`, and CTest | clone and run quick start |

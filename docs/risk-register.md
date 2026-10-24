# Risk register

| ID | Risk | Likelihood | Impact | Trigger | Mitigation |
|---|---|---:|---:|---|---|
| RISK-01 | Assignment later requires native code | Medium | Critical | Rubric mentions machine/native output | Verify in M0; promote C transpiler to MUST and cut all SHOULD work. |
| RISK-02 | Scope grows beyond six weeks | High | High | New feature requested after M2 | Enforce tiers; cut arrays, floats, pipe, fuzzing, then cosmetic output. |
| RISK-03 | Parser and semantic contracts drift | Medium | High | Tests require ad-hoc AST fields | Freeze headers in M0; use AST golden tests before backend work. |
| RISK-04 | Cross-platform warning/build mismatch | Medium | Medium | CI fails on macOS or MSVC only | Build on all three OSes in CI; avoid OS APIs; keep scripts duplicated and simple. |
| RISK-05 | VM call-frame bugs break recursion | Medium | High | Factorial/Fibonacci fail or corrupt stack | Handcrafted bytecode VM tests, stack-height assertions, and sanitizer runs. |
| RISK-06 | Diagnostics arrive too late | Medium | High | Front end works but errors lack spans | Finish diagnostics before lexer/parser; require a negative test per code. |
| RISK-07 | Vendored test framework is incomplete | Low | Medium | Unit runner cannot filter tests | Keep doctest-compatible API tiny and replace with approved official header if needed. |
| RISK-08 | Team merge conflicts in shared headers | Medium | Medium | Multiple branches edit AST or tokens | Directory ownership and contract review before implementation. |

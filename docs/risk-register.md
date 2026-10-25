# Risk register

| ID | Risk | Likelihood | Impact | Trigger | Mitigation |
|---|---|---:|---:|---|---|
| RISK-01 | Assignment later requires native code | Medium | Critical | Rubric mentions machine/native output | Verify in M0; promote C transpiler to MUST and cut all SHOULD work. |
| RISK-02 | Scope grows beyond six weeks | High | High | New feature requested after M2 | Enforce tiers; cut arrays, floats, pipe, fuzzing, then cosmetic output. |
| RISK-03 | Parser and semantic contracts drift | Medium | High | Tests require ad-hoc AST fields | Freeze headers in M0; use AST golden tests before backend work. |
| RISK-04 | Cross-platform warning/build mismatch | Medium | Medium | CI fails on macOS or MSVC only | Build on all three OSes in CI; avoid OS APIs; keep scripts duplicated and simple. |
| RISK-05 | VM call-frame bugs break recursion | Medium | High | Factorial/Fibonacci fail or corrupt stack | Handcrafted bytecode VM tests, stack-height assertions, and sanitizer runs. |
| RISK-06 | Diagnostics arrive too late | Medium | High | Front end works but errors lack spans | Finish diagnostics before lexer/parser; require a negative test per code. |
| RISK-07 | Upstream test framework upgrade changes CLI behavior | Low | Medium | Unit runner filters or assertions stop working after an upgrade | Pin the official header version in `third_party/doctest/README.md` and verify CTest filters in CI. |
| RISK-08 | Team merge conflicts in shared headers | Medium | Medium | Multiple branches edit AST or tokens | Directory ownership and contract review before implementation. |
| RISK-09 | Submission archive omits a required file or includes build state | Low | High | Manifest check finds missing/forbidden path | Generate `submission.zip`, run `scripts/check-package.py`, and rehearse extraction in a clean directory. |
| RISK-10 | Performance evidence is below the informal target | Medium | Medium | Release `fib(30)` exceeds a few seconds or 1,000-line timing is absent | Record the actual result, state the evidence gap, and prioritize correctness over unplanned optimization. |
| RISK-11 | Generic rubric does not match the official assignment | High | Critical | Instructor supplies a different rubric | Keep the traceability matrix generic but explicit, then replace rows one-for-one before submission. |

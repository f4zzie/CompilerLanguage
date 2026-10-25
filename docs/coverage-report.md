# Test coverage inventory

Generated with `python3 scripts/coverage-report.py` on October 25, 2026. “Assertions” means doctest
assertion macros for unit tests; corpus rows count exact output plus exit/stderr checks as two observable
checks per fixture. Fuzz counts the deterministic 1,000-input smoke workload.

| Kind | Cases | Assertions/checks |
|---|---:|---:|
| Unit | 57 | 180 |
| Golden | 3 | 3 |
| End-to-end | 16 | 32 |
| Negative | 26 | 52 |
| Fuzz | 1 workload | 1,000 inputs |

End-to-end includes 12 showcase examples, 3 positive corpus programs, and the walking-skeleton
process test. Negative includes 25 existing error fixtures plus the R004 call-stack-overflow fixture.

## Error-code coverage

| Code | Required negative coverage | Evidence |
|---|---|---|
| L001–L004 | yes | `tests/errors/illegal-character.src`, `unterminated-comment.src`, `unterminated-string.src`, `invalid-escape.src` |
| P001–P003 | yes | `unexpected-token.src`, `missing-semicolon.src`, `invalid-assignment.src` |
| S001–S003 | yes | `duplicate-name.src`, `unresolved-name.src`, `local-forward-reference.src` |
| T001–T008 | yes | matching fixtures in `tests/errors/` |
| R001 | yes | `division-by-zero.kite`, `tests/unit/vm_test.cpp` |
| R002 | yes | `overflow.src` |
| R003 | yes | `index-range.src`, `tests/unit/arrays_for_test.cpp` |
| R004 | yes | `call-stack-overflow.kite`, `tests/unit/recursion_test.cpp` |
| R005 | yes | `bad-to-int.src` |
| R006 | yes | `tests/unit/vm_test.cpp` malformed-bytecode case |

All documented compile-time and runtime codes now have either a corpus negative test or a unit-level
negative test where the condition cannot be produced through the source CLI (R006 malformed bytecode).

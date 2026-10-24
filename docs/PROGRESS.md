# Kite implementation progress

Current task: **T003 — diagnostics and source rendering**

| Task | Status | Branch | Merge commit | Test counts |
|---|---|---|---|---|
| T001 | done | `feat/T001-repo-foundations` | `86d471d` | 1 unit pass; walking gate intentionally pending |
| T002 | done | `feat/T002-specification-and-adrs` | `2aa662f` | 1 unit pass |
| T003 | in progress | `feat/T003-diagnostics` | — | pending |
| T004 | not started | — | — | — |
| T005 | not started | — | — | — |
| T006 | not started | — | — | — |
| T007 | not started | — | — | — |
| T008 | not started | — | — | — |
| T009 | not started | — | — | — |
| T010 | not started | — | — | — |
| T011 | not started | — | — | — |
| T012 | not started | — | — | — |
| T013 | not started | — | — | — |
| T014 | not started | — | — | — |
| T015 | not started | — | — | — |
| T016 | not started | — | — | — |
| T017 | not started | — | — | — |
| T018 | not started | — | — | — |
| T019 | not started | — | — | — |
| T020 | not started | — | — | — |
| T021 | not started | — | — | — |
| T022 | not started | — | — | — |
| T023 | not started | — | — | — |
| T024 | not started | — | — | — |
| T025 | not started | — | — | — |
| T026 | not started | — | — | — |
| T027 | not started | — | — | — |
| T028 | not started | — | — | — |
| T029 | not started | — | — | — |

## Decisions made

- The implementation remains C++20 with the standard library and upstream doctest v2.5.3.
- The default backend remains direct AST-to-bytecode lowering and a stack VM.
- T014 will replace the old lexer-file integration test with a CTest process test.
- The default CTest suite will be fully green after T014; no temporary test exclusion will remain.

## Blockers

- None.

## Next step

Implement bounded diagnostic collection, source lookup, deterministic ordering, and caret rendering
for T003 before starting lexer work.

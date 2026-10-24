# Kite implementation progress

Current task: **T025 — fuzz smoke tests and sanitizer verification**

| Task | Status | Branch | Merge commit | Test counts |
|---|---|---|---|---|
| T001 | done | `feat/T001-repo-foundations` | `86d471d` | 1 unit pass; walking gate intentionally pending |
| T002 | done | `feat/T002-specification-and-adrs` | `2aa662f` | 1 unit pass |
| T003 | done | `feat/T003-diagnostics` | `86f74b3` | 7 cases / 13 assertions; 1 CTest pass |
| T004 | done | `feat/T004-token-model-and-lexer` | `d341459` | 11 default cases / 1 CTest pass |
| T005 | done | `feat/T005-ast-and-printer` | `e313b89` | 13 default cases / 1 CTest pass |
| T006 | done | `feat/T006-pratt-expression-parser` | `7427231` | 15 default cases / 1 CTest pass |
| T007 | done | `feat/T007-statement-parser-and-recovery` | `efb20cb` | 21 default cases / 1 CTest pass |
| T008 | done | `feat/T008-symbol-table-and-scopes` | `f434abe` | 24 default cases / 1 CTest pass |
| T009 | done | `feat/T009-type-system-and-checker` | `f4261de` | 29 default cases / 1 CTest pass |
| T010 | done | `feat/T010-bytecode-format-and-disassembler` | `cde6f9c` | 32 default cases / 1 CTest pass |
| T011 | done | `feat/T011-code-generator` | `45a93fa` (on main) | 33 default cases / 1 CTest pass |
| T012 | done | `feat/T012-vm-core` | `5f0cb12` | 37 default cases / 1 CTest pass |
| T013 | done | `feat/T013-driver-and-cli` | `b71424b` (on main) | 40 default cases / 1 CTest pass |
| T014 | done | `feat/T014-walking-skeleton` | `b32ea55` | 2 CTest tests pass |
| T015 | done | `feat/T015-variables-and-scopes` | `f4a306b` | 42 default cases / 2 CTest tests pass |
| T016 | done | `feat/T016-control-flow` | `b288b1b` | 45 default cases / 2 CTest tests pass |
| T017 | done | `feat/T017-functions-and-call-frames` | `4af1c1d` | 48 default cases / 2 CTest tests pass |
| T018 | done | `feat/T018-recursion-and-returns` | `32a9140` | 51 default cases / 2 CTest tests pass |
| T019 | done | `feat/T019-strings-and-builtins` | `c1c4aec` | 54 default cases / 2 CTest tests pass |
| T020 | done | `feat/T020-pipe-operator` | `2eca8ce` | 56 default cases / 2 CTest tests pass |
| T021 | done | `feat/T021-arrays-floats-and-for` | `adaf3a6` | 57 default cases / 2 CTest tests pass |
| T022 | done | `feat/T022-golden-test-fixtures` | `2346692` | 57 default cases / 5 CTest tests pass |
| T023 | done | `feat/T023-end-to-end-runner` | `bc27066` | 57 default cases / 5 CTest tests / 3 corpus passes |
| T024 | done | `feat/T024-negative-test-corpus` | `1ad1c55` | 57 default cases / 5 CTest / 3 positive + 2 negative corpus passes |
| T025 | in progress | `feat/T025-fuzz-and-sanitizers` | — | pending |
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

Add deterministic bounded lexer/parser crash-resistance fuzzing and sanitizer build verification.

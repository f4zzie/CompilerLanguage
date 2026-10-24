ID:              T024
Title:           Complete negative test corpus
Milestone:       M4
Tier:            MUST
Depends on:      [T003, T004, T007, T009, T023]
Blocks:          [T025, T027]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   tests/errors/*; docs/error-and-diagnostics.md; docs/testing-strategy.md
Description:     Add at least one program for every compile-time error code and every required runtime error, with exact code/location/message fragments.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is invalid source or bytecode. Output is stable stderr and exit code 1 or 3.
Acceptance criteria:   1. L/P/S/T codes each have a test. 2. R001 and R002 have runtime tests. 3. Caret appears. 4. Multi-error source respects max-errors.
Tests to write first: names and what they assert: `errors_all_compile_codes`; `errors_runtime_codes`; `errors_max_errors`; assert code, span, and exit.
Edge cases to cover: first-byte error; final-byte error; multiple errors on one line; parser recovery after malformed block; cascaded type errors.
Out of scope:          localized messages, JSON output, IDE protocol.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

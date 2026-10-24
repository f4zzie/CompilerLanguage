ID:              T022
Title:           Golden token, AST, and disassembly fixtures
Milestone:       M1/M6
Tier:            MUST
Depends on:      [T004, T005, T010, T014]
Blocks:          [T026]
Owner:           (unassigned)
Estimate:        3 / 4 / 6 hours
Files to create or modify:   tests/golden/*; scripts/update-golden.sh; scripts/update-golden.ps1; docs/testing-strategy.md
Description:     Create representative source fixtures and a deterministic regeneration/review workflow for every dump stage.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is fixture source. Output is checked token, AST, and disassembly text.
Acceptance criteria:   1. At least hello, expressions, control flow, and functions have all three dumps. 2. Regeneration is opt-in. 3. CTest reports diffs clearly.
Tests to write first: names and what they assert: `golden_hello`; `golden_control_flow`; `golden_functions`; assert byte-for-byte snapshots.
Edge cases to cover: empty source; multiple functions; nested blocks; strings with escapes; stable ordering across repeated runs.
Out of scope:          visual snapshot tooling and binary bytecode snapshots.
Definition of done:    code merged, tests green, docs updated, dump flag works

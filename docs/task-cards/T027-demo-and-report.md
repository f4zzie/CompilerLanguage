ID:              T027
Title:           Final demo and written report
Milestone:       M7
Tier:            MUST
Depends on:      [T018, T019, T023, T024, T026]
Blocks:          [T028, T029]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   docs/demo-and-report-plan.md; report/outline.md; report/evidence.md; examples/*
Description:     Rehearse the prescribed demo, capture evidence, and write the report sections using actual test/build results rather than planned claims.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is the final executable, corpus, and docs. Output is a reproducible demo script and report outline/evidence.
Acceptance criteria:   1. Demo fits ten minutes. 2. Shows a successful run, dump stages, trace, compile error, runtime error, and tests. 3. Report cites file paths and commands.
Tests to write first: names and what they assert: `demo_smoke`; `report_evidence_check`; assert all referenced examples and commands exist.
Edge cases to cover: SHOULD pipe cut; clean-machine setup; missing optional sanitizer; terminal output wrapping.
Out of scope:          adding features during rehearsal, unverified performance claims, external publishing.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

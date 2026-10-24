ID:              T026
Title:           README and showcase example corpus
Milestone:       M6
Tier:            MUST
Depends on:      [T019, T022, T023]
Blocks:          [T027, T028]
Owner:           (unassigned)
Estimate:        3 / 4 / 6 hours
Files to create or modify:   README.md; examples/*.kite; docs/repo-structure.md; docs/language-spec.md
Description:     Keep the user-facing README aligned with the actual CLI, add the complete showcase corpus, and verify every documented command/path.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is the implemented command set and sample programs. Output is runnable documentation with exact outputs.
Acceptance criteria:   1. Every example has an expected output. 2. README quick start works from a clean build directory. 3. SHOULD examples are clearly labeled. 4. No documented command is missing.
Tests to write first: names and what they assert: `examples_all_run`; `readme_commands`; assert corpus outputs and command exit codes.
Edge cases to cover: source path with spaces; empty input; error examples; trace output separated from stdout; Windows PowerShell invocation.
Out of scope:          course report prose, website hosting, generated screenshots.
Definition of done:    code merged, tests green, docs updated, dump flag works

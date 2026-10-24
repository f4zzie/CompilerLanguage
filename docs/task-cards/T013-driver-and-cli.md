ID:              T013
Title:           Driver pipeline and CLI
Milestone:       M1
Tier:            MUST
Depends on:      [T003, T004, T006, T009, T011, T012]
Blocks:          [T014, T022, T023]
Owner:           (unassigned)
Estimate:        4 / 6 / 7 hours
Files to create or modify:   src/driver/main.cpp; src/driver/pipeline.*; tests/unit/driver_test.cpp
Description:     Implement hand-rolled command parsing, source loading, pipeline orchestration, stage dumps, flags, and exit-code mapping.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is argv and a source path. Output is stdout/stderr plus exit code 0–4.
Acceptance criteria:   1. All six commands parse. 2. `--trace` and `--max-errors` reach the pipeline. 3. Missing files return 2. 4. Compile errors return 1 and runtime errors return 3.
Tests to write first: names and what they assert: `cli_help`; `cli_missing_file`; `cli_dump_commands`; assert usage text and exit codes.
Edge cases to cover: no arguments; unknown command; option before/after file; empty source; Windows path separators.
Out of scope:          shell completion, config files, third-party CLI libraries.
Definition of done:    code merged, tests green, docs updated, dump flag works

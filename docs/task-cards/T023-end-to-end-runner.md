ID:              T023
Title:           Automated end-to-end test runner
Milestone:       M2/M6
Tier:            MUST
Depends on:      [T013, T014]
Blocks:          [T026, T027]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   scripts/test-all; scripts/test-all.ps1; tests/programs/*; CMakeLists.txt
Description:     Implement the corpus runner for `.src`, `.expected`, `.exitcode`, `.stdin`, and optional `.stderr` files with bounded process time.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is a built `kite` binary and corpus directory. Output is pass/fail summary and nonzero exit on mismatch.
Acceptance criteria:   1. Positive programs compare exact stdout. 2. Exit codes are checked. 3. Stdin fixtures are supplied. 4. Timeout failures identify the program. 5. Shell and PowerShell paths work.
Tests to write first: names and what they assert: `runner_hello`; `runner_runtime_error`; `runner_timeout`; assert status and diff reporting.
Edge cases to cover: missing expected file; empty expected output; CRLF normalization; spaces in paths; program writes stderr.
Out of scope:          networked test services, Docker, external test runners.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

ID:              T028
Title:           Submission package and clean-machine rehearsal
Milestone:       M7
Tier:            MUST
Depends on:      [T026, T027]
Blocks:          [T029]
Owner:           (unassigned)
Estimate:        2 / 4 / 6 hours
Files to create or modify:   scripts/package-submission.sh; scripts/package-submission.ps1; docs/demo-and-report-plan.md; README.md
Description:     Create an archive containing source, docs, tests, examples, scripts, CMake files, and the approved vendored test header while excluding builds and private files.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is the repository at feature freeze. Output is a clean archive that configures, builds, and tests.
Acceptance criteria:   1. Archive contains every required deliverable. 2. Archive contains no `build/`, credentials, logs, or editor state. 3. Clean extraction passes configure/build and the expected scaffold or final test suite.
Tests to write first: names and what they assert: `package_manifest`; `package_clean_build`; assert paths and reproducibility.
Edge cases to cover: spaces in extraction directory; Windows archive path handling; missing optional report format; empty build directory.
Out of scope:          upload, email, publishing, or payment actions.
Definition of done:    code merged, tests green, docs updated, dump flag works

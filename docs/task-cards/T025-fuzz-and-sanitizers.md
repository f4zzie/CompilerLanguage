ID:              T025
Title:           Fuzz smoke tests and sanitizer verification
Milestone:       M6
Tier:            SHOULD
Depends on:      [T014, T023, T024]
Blocks:          [T027]
Owner:           (unassigned)
Estimate:        3 / 5 / 7 hours
Files to create or modify:   tests/fuzz/lexer_parser_fuzz.cpp; CMakeLists.txt; docs/testing-strategy.md; scripts/test-all
Description:     Add bounded random/fuzz input coverage and run the Linux ASan/UBSan preset over unit and corpus tests.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is arbitrary bytes. Output is no crash/hang within the timeout and a recorded sanitizer result.
Acceptance criteria:   1. 1,000 deterministic random inputs complete. 2. Parser never dereferences a null child. 3. Sanitizer build configures and runs. 4. Any discovered crash becomes a regression test.
Tests to write first: names and what they assert: `fuzz_lexer_parser_no_crash`; `sanitizer_smoke`; assert completion and zero sanitizer findings.
Edge cases to cover: empty bytes; invalid UTF-8; huge token; unterminated comment; deeply nested parentheses; repeated invalid bytes.
Out of scope:          long-running libFuzzer campaigns, performance fuzzing, security certification.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

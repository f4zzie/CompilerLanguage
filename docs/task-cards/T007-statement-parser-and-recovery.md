ID:              T007
Title:           Statement parser and recovery
Milestone:       M1
Tier:            MUST
Depends on:      [T004, T005, T006]
Blocks:          [T008, T014, T016, T017]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/parser/parser.*; tests/unit/parser_statement_test.cpp; tests/errors/*.src
Description:     Parse top-level functions and statements, require semicolons, build blocks, and synchronize after recoverable syntax errors.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is tokens. Output is a non-null `Program` plus zero or more P001–P003 diagnostics.
Acceptance criteria:   1. All grammar productions parse. 2. Parser continues after one malformed statement. 3. Missing delimiters have correct spans. 4. Empty program and nested blocks parse.
Tests to write first: names and what they assert: `parser_function`; `parser_control_flow`; `parser_recovery`; assert node count and diagnostic codes.
Edge cases to cover: missing `}`; missing `;`; `else` without `if`; return at top level; empty parameter list; nested `else if`.
Out of scope:          symbol lookup, type rules, bytecode generation.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

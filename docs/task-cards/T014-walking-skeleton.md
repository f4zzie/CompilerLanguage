ID:              T014
Title:           Walking skeleton integration
Milestone:       M1
Tier:            MUST
Depends on:      [T011, T012, T013]
Blocks:          [T015, T016, T017, T022, T023]
Owner:           (unassigned)
Estimate:        3 / 4 / 6 hours
Files to create or modify:   tests/unit/walking_skeleton_test.cpp; tests/programs/hello.src; tests/programs/hello.expected; tests/programs/hello.exitcode
Description:     Replace the intentional red test with a complete `print(1 + 2);` path through lexer, parser, sema, codegen, VM, and driver.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is `print(1 + 2);`. Output is stdout `3\n`, exit code 0, and a usable token/AST/disassembly dump.
Acceptance criteria:   1. The walking-skeleton CTest passes. 2. The program output is exactly `3`. 3. A malformed source exits 1. 4. No stage contains a placeholder for this path.
Tests to write first: names and what they assert: `walking skeleton`; assert exact output and zero exit status.
Edge cases to cover: parenthesized arithmetic; integer literal at EOF; expression statement discard; empty program.
Out of scope:          loops, functions, strings, arrays, optimization.
Definition of done:    code merged, tests green, docs updated, dump flag works

ID:              T014
Title:           Walking skeleton integration
Milestone:       M1
Tier:            MUST
Depends on:      [T011, T012, T013]
Blocks:          [T015, T016, T017, T022, T023]
Owner:           (unassigned)
Estimate:        3 / 4 / 6 hours
Files to create or modify:   CMakeLists.txt; tests/e2e/walking_skeleton_test.cmake; tests/unit/lexer_test.cpp; examples/hello.kite; examples/hello-world.kite; README.md; docs/language-spec.md; docs/demo-and-report-plan.md; docs/rubric-traceability.md
Description:     Replace the integration case currently embedded in `tests/unit/lexer_test.cpp` with a CTest end-to-end test. The CMake test script must execute the built `kite` binary with `run examples/hello.kite`, capture stdout/stderr and the exit code, and compare the exact expected output. Remove the `KITE_RUN_WALKING_SKELETON` switch and its `walking_skeleton` exclusion so this test runs in the default CTest suite. Keep the arithmetic walking-skeleton source as `print(1 + 2);`; move the current greeting sample to `examples/hello-world.kite` and update every affected document.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is `examples/hello.kite` containing `print(1 + 2);`. Output is stdout `3\n`, empty failure stderr, exit code 0, and a usable token/AST/disassembly dump.
Acceptance criteria:   1. The walking-skeleton CTest invokes the `kite` executable on `examples/hello.kite`. 2. The test passes by default without a special switch. 3. The program output is exactly `3`. 4. A malformed source exits 1. 5. No stage contains a placeholder for this path. 6. `KITE_RUN_WALKING_SKELETON`, the test-case exclusion, and the lexer-unit integration case are removed.
Tests to write first: names and what they assert: `tests/e2e/walking_skeleton_test.cmake`; assert the `kite` process exits 0 and stdout is exactly `3\n`.
Edge cases to cover: parenthesized arithmetic; integer literal at EOF; expression statement discard; empty program.
Out of scope:          loops, functions, strings, arrays, optimization.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

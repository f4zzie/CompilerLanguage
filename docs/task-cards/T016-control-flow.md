ID:              T016
Title:           Conditionals and while loops
Milestone:       M2
Tier:            MUST
Depends on:      [T015]
Blocks:          [T022, T023]
Owner:           (unassigned)
Estimate:        5 / 6 / 8 hours
Files to create or modify:   src/sema/sema.cpp; src/codegen/code_generator.cpp; src/runtime/vm.cpp; tests/programs/conditionals.src; tests/programs/loops.src
Description:     Implement boolean conditions, short-circuit `and/or`, if/else jump patching, while back edges, and block scopes.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is typed control-flow AST. Output is balanced conditional/back-edge bytecode.
Acceptance criteria:   1. Conditionals choose exactly one branch. 2. `and/or` short-circuit side effects. 3. While loops terminate and update locals. 4. Non-bool conditions emit T002.
Tests to write first: names and what they assert: `conditional_branch`; `short_circuit`; `while_loop`; assert exact stdout and diagnostics.
Edge cases to cover: false branch absent; zero-iteration loop; nested loops; condition with function call; integer comparison boundary.
Out of scope:          break/continue, expression-oriented blocks, for loops.
Definition of done:    code merged, tests green, docs updated, dump flag works

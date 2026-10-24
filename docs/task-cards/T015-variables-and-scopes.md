ID:              T015
Title:           Variables, assignments, and lexical scopes
Milestone:       M2
Tier:            MUST
Depends on:      [T008, T011, T014]
Blocks:          [T016, T017, T019]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/sema/sema.cpp; src/codegen/code_generator.cpp; src/runtime/vm.cpp; tests/programs/scope.src; tests/errors/duplicate-name.src
Description:     Add local/global slot allocation, mutable `let`, assignment, shadowing, and lexical block entry/exit.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is annotated `LetStmt`/`AssignStmt`. Output is slot-addressed bytecode and updated VM frames.
Acceptance criteria:   1. `examples/scope.kite` prints `2` then `1`. 2. Assignment updates the nearest binding. 3. Same-scope duplicate emits S001. 4. Local forward reference emits S003.
Tests to write first: names and what they assert: `scope_shadowing`; `scope_assignment`; `scope_duplicate`; assert output or exact diagnostics.
Edge cases to cover: assignment in nested block; global variable; parameter reassignment; shadowed function name; assignment result is unit.
Out of scope:          immutable bindings, destructuring, modules.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

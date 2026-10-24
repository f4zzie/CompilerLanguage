ID:              T017
Title:           Functions, parameters, and call frames
Milestone:       M3
Tier:            MUST
Depends on:      [T008, T011, T012, T015]
Blocks:          [T018, T019, T020]
Owner:           (unassigned)
Estimate:        6 / 8 / 8 hours
Files to create or modify:   src/sema/sema.cpp; src/codegen/code_generator.cpp; src/runtime/vm.cpp; tests/programs/functions.src; tests/errors/wrong-arity.src
Description:     Add function table entries, parameter binding, CALL/RETURN emission, frame-local slots, and exact arity/type checking.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is valid `FunctionDecl` nodes. Output is callable bytecode functions with isolated frames.
Acceptance criteria:   1. Multiple parameters bind in order. 2. Calls return values. 3. Wrong arity emits T004. 4. Wrong argument type emits T005. 5. Nested calls preserve locals.
Tests to write first: names and what they assert: `function_parameters`; `function_return`; `function_wrong_arity`; assert output or diagnostic.
Edge cases to cover: zero parameters; unit return; parameter name shadowing; call inside condition; function declaration after caller.
Out of scope:          closures, first-class function values, variadic calls.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

ID:              T020
Title:           Pipe operator SHOULD feature
Milestone:       M5
Tier:            SHOULD
Depends on:      [T017, T019]
Blocks:          [T027]
Owner:           (unassigned)
Estimate:        2 / 3 / 4 hours
Files to create or modify:   src/lexer/lexer.cpp; src/parser/parser.cpp; src/sema/sema.cpp; src/codegen/code_generator.cpp; tests/programs/pipe.src; tests/errors/pipe-target.src
Description:     Implement `value |> function_name` as left-to-right insertion of the value as the first function argument.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is `PipeExpr`. Output is ordinary CALL bytecode with the value pushed before other arguments.
Acceptance criteria:   1. `5 |> double |> print` prints `10`. 2. Pipe target must resolve to a function. 3. Invalid target has a stable diagnostic. 4. Pipe remains classified SHOULD in all docs.
Tests to write first: names and what they assert: `pipe_chain`; `pipe_invalid_target`; assert output and diagnostic.
Edge cases to cover: zero-parameter target; target with incompatible first parameter; nested pipe; function name shadowing.
Out of scope:          arbitrary lambda RHS, multi-argument pipe syntax, changing the tier to MUST.
Definition of done:    code merged, tests green, docs updated, dump flag works

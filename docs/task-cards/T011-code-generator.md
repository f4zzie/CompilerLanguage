ID:              T011
Title:           Typed AST to bytecode generator
Milestone:       M1
Tier:            MUST
Depends on:      [T009, T010]
Blocks:          [T014, T015, T016, T017]
Owner:           (unassigned)
Estimate:        6 / 8 / 8 hours
Files to create or modify:   src/codegen/code_generator.*; tests/unit/codegen_test.cpp; tests/golden/*.disasm
Description:     Emit constants, arithmetic, comparisons, local/global access, branches, calls, returns, and halt instructions from an annotated AST.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is a semantically valid `Program`. Output is a deterministic `BytecodeModule` with balanced stack effects.
Acceptance criteria:   1. Every MUST expression and statement emits bytecode. 2. Jumps are patched. 3. Function entries and arities are correct. 4. Disassembly fixtures are stable.
Tests to write first: names and what they assert: `codegen_literal_expression`; `codegen_branch`; `codegen_function`; assert opcode sequences and operands.
Edge cases to cover: empty block; implicit unit expression result; nested branches; global versus local binding; unreachable return after a branch.
Out of scope:          peephole optimization, arrays, C backend, VM dispatch.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

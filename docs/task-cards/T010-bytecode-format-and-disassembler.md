ID:              T010
Title:           Bytecode module and disassembler
Milestone:       M1
Tier:            MUST
Depends on:      [T002, T003]
Blocks:          [T011, T012, T014]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/codegen/bytecode.*; tests/unit/bytecode_test.cpp; tests/golden/*.disasm
Description:     Finalize opcode enum, constants, function table, instruction spans, stack-effect documentation, and deterministic disassembly.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is emitted `Instruction` objects. Output is a self-contained `BytecodeModule` and text dump.
Acceptance criteria:   1. Every opcode in architecture.md exists. 2. Disassembly includes index/opcode/operand. 3. Constants and function metadata are stable. 4. Invalid operand indexes are detectable by the VM.
Tests to write first: names and what they assert: `bytecode_disassemble`; `bytecode_constant_pool`; `bytecode_function_metadata`; assert exact output.
Edge cases to cover: empty module; zero-arity function; jump to first/last instruction; duplicate constants; empty string constant.
Out of scope:          AST lowering and execution dispatch.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

ID:              T012
Title:           Stack VM and call-frame core
Milestone:       M1/M3
Tier:            MUST
Depends on:      [T010]
Blocks:          [T014, T017, T018]
Owner:           (unassigned)
Estimate:        6 / 8 / 8 hours
Files to create or modify:   src/runtime/value.*; src/runtime/vm.*; tests/unit/vm_test.cpp
Description:     Implement the dispatch loop, operand stack, local/global access, arithmetic, jumps, halt, and frame structures defined by the bytecode contract.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is `BytecodeModule`. Output is `VmResult` with value or `RuntimeError`.
Acceptance criteria:   1. Handcrafted constant/add/halt module returns 3. 2. Branch instructions select the correct path. 3. Stack underflow and malformed modules return R006. 4. Trace prints to stderr.
Tests to write first: names and what they assert: `vm_adds_integers`; `vm_branches`; `vm_rejects_bad_operand`; `vm_trace`; assert result/error and trace text.
Edge cases to cover: empty module; division by zero; signed overflow; maximum stack; return from synthetic main; invalid function index.
Out of scope:          source parsing, builtins, arrays, garbage collection.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

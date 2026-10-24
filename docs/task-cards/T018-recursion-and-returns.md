ID:              T018
Title:           Recursion and return-path checking
Milestone:       M3
Tier:            MUST
Depends on:      [T017]
Blocks:          [T023, T027]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/sema/sema.cpp; src/runtime/vm.cpp; tests/programs/recursion.src; tests/errors/missing-return.src
Description:     Verify recursive calls, nested frame restoration, all-path return checking, and call-stack overflow reporting.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is recursive function AST. Output is correct result or R004/T008.
Acceptance criteria:   1. Factorial(5) returns 120. 2. Fibonacci(6) returns 8. 3. Missing returns emit T008. 4. Excessive recursion emits R004 without memory corruption.
Tests to write first: names and what they assert: `recursive_factorial`; `recursive_fibonacci`; `missing_return`; `stack_overflow`; assert result/error.
Edge cases to cover: base case at zero; negative input; recursive function with locals; unit recursive function; return inside branch.
Out of scope:          tail-call optimization, closures, coroutines.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

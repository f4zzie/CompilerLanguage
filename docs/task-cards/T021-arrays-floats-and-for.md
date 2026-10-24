ID:              T021
Title:           Arrays, floats, and C-style for SHOULD features
Milestone:       M5
Tier:            SHOULD
Depends on:      [T016, T019]
Blocks:          []
Owner:           (unassigned)
Estimate:        6 / 8 / 8 hours
Files to create or modify:   src/parser/ast.hpp; src/sema/*; src/codegen/*; src/runtime/*; tests/programs/arrays-for.src; tests/programs/float.src; tests/errors/index-range.src
Description:     Add primitive float values, homogeneous array literals/indexing, and C-style for lowering only after all MUST tests pass.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is typed array/float AST. Output is checked `Value` storage and loop bytecode.
Acceptance criteria:   1. `arrays-for` prints 6. 2. Float arithmetic prints 3.5. 3. Index errors emit R003. 4. MUST test suite remains green.
Tests to write first: names and what they assert: `array_literal_and_index`; `array_index_error`; `float_arithmetic`; `for_loop`; assert output/errors.
Edge cases to cover: empty array; negative index; mixed element types; zero-iteration for; float divide by zero; array aliasing.
Out of scope:          records, multidimensional arrays, slicing, generics, garbage collection.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

ID:              T005
Title:           AST definitions and canonical printer
Milestone:       M1
Tier:            MUST
Depends on:      [T002, T003]
Blocks:          [T006, T007, T009, T011]
Owner:           (unassigned)
Estimate:        3 / 5 / 7 hours
Files to create or modify:   src/parser/ast.hpp; src/parser/ast_printer.*; tests/unit/ast_test.cpp; tests/golden/*.ast
Description:     Define all expression, statement, declaration, type, span, and node-ID structures and implement a stable tree dump.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is parser-created ownership tree. Output is a canonical text representation used by golden tests.
Acceptance criteria:   1. Every grammar production maps to a node or field. 2. No owning raw pointers exist. 3. Printer includes node kind, names, literals, and spans. 4. Nested ownership destroys cleanly.
Tests to write first: names and what they assert: `ast_printer_literals`; `ast_printer_function`; asserts exact canonical output.
Edge cases to cover: empty program; empty block; function with no parameters; return without value; nested pipe/call/index expressions.
Out of scope:          parsing, semantic annotations beyond storage, serialization formats.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

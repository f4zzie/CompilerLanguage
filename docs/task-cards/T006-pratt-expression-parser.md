ID:              T006
Title:           Pratt expression parser
Milestone:       M1
Tier:            MUST
Depends on:      [T004, T005]
Blocks:          [T007, T009, T014]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/parser/precedence.hpp; src/parser/parser.*; tests/unit/parser_expression_test.cpp; tests/golden/*.ast
Description:     Implement primary, postfix, unary, binary, assignment, short-circuit, and SHOULD pipe parsing using the documented binding powers.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is the token vector. Output is an expression subtree with spans and no semantic annotations.
Acceptance criteria:   1. Every operator has the specified precedence. 2. Assignment is right-associative. 3. Calls and indexes chain. 4. Invalid assignment targets emit P003. 5. AST golden files pass.
Tests to write first: names and what they assert: `parser_precedence`; `parser_assignment`; `parser_call_and_index`; `parser_pipe`; assert AST shape.
Edge cases to cover: unary minus versus subtraction; empty argument list; trailing comma rejection; nested parentheses; pipe chain associativity.
Out of scope:          statement parsing, name resolution, type checking, bytecode.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

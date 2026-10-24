ID:              T002
Title:           Freeze draft specification and ADRs
Milestone:       M0
Tier:            MUST
Depends on:      [T001]
Blocks:          [T003, T004, T005, T006, T008, T010]
Owner:           (unassigned)
Estimate:        3 / 5 / 8 hours
Files to create or modify:   docs/language-spec.md; docs/assumptions.md; docs/decisions/*.md; PLAN.md
Description:     Review grammar, type rules, feature tiers, error codes, and backend decisions. Resolve naming across tokens, AST nodes, opcodes, examples, and tests.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is the approved project parameters. Output is the normative grammar and contracts used by all implementation cards.
Acceptance criteria:   1. Every reserved word has a TokenKind. 2. Operator precedence matches the grammar. 3. Every MUST feature has a positive and negative test name. 4. Pipe is marked SHOULD exactly once. 5. Native-code contingency is documented.
Tests to write first: names and what they assert: `tests/spec/contract-checklist.md`; asserts grammar/operator/feature-tier consistency by checklist.
Edge cases to cover: unit return without expression; function forward references; mutable shadowing; required semicolons; empty blocks; error cap.
Out of scope:          implementing any compiler stage or adding unapproved language features.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

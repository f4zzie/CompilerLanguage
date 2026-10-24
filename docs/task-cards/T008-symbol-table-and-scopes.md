ID:              T008
Title:           Symbol table and lexical scopes
Milestone:       M2
Tier:            MUST
Depends on:      [T005, T007]
Blocks:          [T009, T015, T017]
Owner:           (unassigned)
Estimate:        3 / 4 / 6 hours
Files to create or modify:   src/sema/symbol.*; tests/unit/symbol_test.cpp
Description:     Implement insertion, lexical lookup, scope push/pop, function predeclaration, duplicate detection, and local-slot metadata.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is names and declaration spans. Output is deterministic `Symbol` lookup results.
Acceptance criteria:   1. Same-scope duplicates are rejected. 2. Nested shadowing works. 3. Parent lookup works. 4. Scope destruction leaves no dangling lookup state. 5. Function names can be predeclared.
Tests to write first: names and what they assert: `symbol_shadowing`; `symbol_duplicate`; `symbol_parent_lookup`; assert returned declaration spans.
Edge cases to cover: empty scope; lookup after pop; global/function name collision; local used before declaration; many nested scopes.
Out of scope:          expression type inference, bytecode local-slot emission.
Definition of done:    code merged, tests green, docs updated, dump flag works

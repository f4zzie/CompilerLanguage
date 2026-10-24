ID:              T009
Title:           Type representation and semantic checker
Milestone:       M2
Tier:            MUST
Depends on:      [T007, T008]
Blocks:          [T011, T014, T016, T017, T019]
Owner:           (unassigned)
Estimate:        6 / 8 / 8 hours
Files to create or modify:   src/sema/type.*; src/sema/sema.*; tests/unit/sema_test.cpp; tests/errors/*.src
Description:     Add builtin/function signatures, name resolution, local inference, operator typing, condition checks, call checks, and return-path checks.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is `Program&`. Output is annotated expression types and deterministic S/T diagnostics.
Acceptance criteria:   1. Every MUST type rule is implemented. 2. Recursion and function forward references work. 3. Errors recover with `Error` types. 4. Every T/S code has a negative test.
Tests to write first: names and what they assert: `sema_infers_let`; `sema_checks_operators`; `sema_checks_calls`; `sema_checks_returns`; assert types and codes.
Edge cases to cover: unit return; missing return; shadowed name; zero-argument function; wrong arity; invalid pipe target; bool in numeric expression.
Out of scope:          implicit conversions, generics, structs, closures, optimization.
Definition of done:    code merged, all tests pass, docs updated, dump flag works

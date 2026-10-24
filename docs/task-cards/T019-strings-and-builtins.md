ID:              T019
Title:           Strings and builtin registry
Milestone:       M4/M5
Tier:            MUST
Depends on:      [T009, T015, T017]
Blocks:          [T020, T023, T027]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/runtime/value.*; src/runtime/builtins.*; src/sema/sema.cpp; src/codegen/code_generator.cpp; tests/programs/strings.src; tests/programs/builtins.src
Description:     Add string literals, concatenation, value display, `print`, `input`, `len`, `to_string`, and `to_int`.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is typed string/builtin AST. Output is string `Value` operations and runtime builtin results.
Acceptance criteria:   1. Escaped strings round-trip. 2. String `+` works. 3. `len` is deterministic. 4. Invalid `to_int` emits R005. 5. Builtins have type-checked signatures.
Tests to write first: names and what they assert: `strings_concat`; `strings_escapes`; `builtin_len`; `builtin_bad_to_int`; assert stdout/errors.
Edge cases to cover: empty string; quote/backslash escapes; input EOF; very long string; print unit; to_string bool.
Out of scope:          mutable strings, Unicode normalization, formatting libraries.
Definition of done:    code merged, tests green, docs updated, dump flag works

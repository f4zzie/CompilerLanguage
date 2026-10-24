ID:              T003
Title:           Diagnostics and source rendering
Milestone:       M0
Tier:            MUST
Depends on:      [T001, T002]
Blocks:          [T004, T006, T007, T009, T013]
Owner:           (unassigned)
Estimate:        3 / 5 / 7 hours
Files to create or modify:   src/common/source.hpp; src/diagnostics/diagnostic.*; src/diagnostics/source_manager.*; tests/unit/diagnostics_test.cpp
Description:     Implement spans, source lookup, bounded diagnostic collection, deterministic sorting, and caret/underline rendering.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is file/source text and diagnostic events. Output is stable `Diagnostic` values and rendered stderr text.
Acceptance criteria:   1. One-line and multi-line spans render correctly. 2. Sorting is stable by file/line/column/code. 3. `max_errors` stops additional errors. 4. Unit tests cover empty and end-of-file spans.
Tests to write first: names and what they assert: `diagnostic_render_single_line`; `diagnostic_render_multiline`; `diagnostic_cap`; asserts exact text and counts.
Edge cases to cover: zero-width spans; span at final byte; missing source file; Unicode bytes before a caret; warning does not count as an error.
Out of scope:          parser recovery, colored terminal output, JSON diagnostics.
Definition of done:    code merged, tests green, docs updated, dump flag works

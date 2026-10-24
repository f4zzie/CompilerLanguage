ID:              T004
Title:           Token model and lexer
Milestone:       M1
Tier:            MUST
Depends on:      [T002, T003]
Blocks:          [T006, T007, T014]
Owner:           (unassigned)
Estimate:        4 / 6 / 8 hours
Files to create or modify:   src/lexer/token.*; src/lexer/lexer.*; tests/unit/lexer_test.cpp; tests/golden/*.tokens
Description:     Implement cursor-based scanning for identifiers, keywords, literals, comments, operators, delimiters, EOF, and lexical diagnostics.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is UTF-8 source text. Output is ordered `std::vector<Token>` ending in one EOF.
Acceptance criteria:   1. All grammar spellings tokenize. 2. Longest-match operators work. 3. Escapes decode. 4. Comments are skipped. 5. L001–L004 have tests. 6. Token golden dump is deterministic.
Tests to write first: names and what they assert: `lexer_keywords`; `lexer_literals`; `lexer_operators`; `lexer_comments`; `lexer_errors`; assert kinds, lexemes, spans, and codes.
Edge cases to cover: empty source; CRLF; unterminated string/comment; invalid escape; identifier adjacent to digits; `->` versus `-`.
Out of scope:          AST construction, type checking, indentation-sensitive syntax.
Definition of done:    code merged, all tests except walking_skeleton pass until T014, docs updated, dump flag works

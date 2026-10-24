# T002 contract checklist

This checklist is the first T002 test artifact. It is reviewed before every front-end contract
change and is intentionally kept as plain text so it can be checked on a clean machine.

- [ ] Every reserved word in `docs/language-spec.md` has a `TokenKind`.
- [ ] Every operator has one precedence and associativity row.
- [ ] Every grammar production has an AST representation or an explicitly documented lowering.
- [ ] Every MUST feature has one positive and one negative test name in `docs/testing-strategy.md`.
- [ ] Every compile-time error code has a message template and a negative fixture.
- [ ] Every bytecode opcode has a stack effect, semantic description, and producer.
- [ ] The default compiler path has no external dependency other than C++20 and doctest v2.5.3.
- [ ] The walking-skeleton gate is the only intentionally incomplete test before T014.

## Review result

T002 implementation review: pending until the lexer, parser, semantic, bytecode, and runtime public
headers are updated to match the contracts.

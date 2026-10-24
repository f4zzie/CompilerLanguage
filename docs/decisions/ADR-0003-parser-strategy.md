# ADR-0003: Recursive descent plus Pratt parsing

## Context

The assignment permits hand-written parsing and forbids parser generators. Expression precedence
needs to be easy to extend and error recovery must produce useful student-facing diagnostics.

## Options considered

1. Recursive descent for declarations/statements plus Pratt expressions.
2. Hand-written recursive descent for every precedence level.
3. A parser generator or PEG library.

## Criteria and scoring

Criteria are allowed tooling 4, grammar clarity 3, extension cost 2, error recovery 3, and team
learning value 2.

| Option | Tooling | Clarity | Extension | Recovery | Learning | Total |
|---|---:|---:|---:|---:|---:|---:|
| RD + Pratt | 5 | 5 | 5 | 4 | 5 | 61 |
| RD levels | 5 | 4 | 3 | 4 | 5 | 52 |
| Generator/PEG | 1 | 4 | 4 | 3 | 2 | 36 |

## Decision

Use recursive descent for top-level items, statements, and types; use Pratt binding powers for
expressions. Synchronize at semicolon, closing brace, EOF, and declaration keywords.

## Consequences

The parser stays dependency-free and the precedence table is visible in one place. The team must
write and maintain recovery logic and AST construction manually.

## How to reverse

Keep token and AST contracts, then replace only the parser implementation. A generator is not
permitted without an assignment-rule change and a new ADR.

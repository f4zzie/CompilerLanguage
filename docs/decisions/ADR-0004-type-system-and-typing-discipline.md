# ADR-0004: Static typing with local inference

## Context

Semantic analysis should be substantial enough for a compiler course while remaining small enough
for one student. Function boundaries benefit from explicit contracts; local declarations benefit
from avoiding repeated type syntax.

## Options considered

1. Dynamic typing.
2. Fully explicit static typing.
3. Static typing with explicit function signatures and inferred local `let` types.

## Criteria and scoring

Criteria are semantic depth 4, implementation effort 3, error quality 3, syntax usability 2, and
runtime simplicity 2.

| Option | Depth | Effort | Errors | Usability | Runtime | Total |
|---|---:|---:|---:|---:|---:|---:|
| Dynamic | 2 | 5 | 2 | 5 | 2 | 45 |
| Fully explicit | 5 | 3 | 5 | 2 | 5 | 52 |
| Local inference | 5 | 4 | 5 | 4 | 5 | 63 |

## Decision

Use `int`, `bool`, `string`, and `unit` in MUST scope, with float and arrays as SHOULD. Require
parameter and return annotations; infer a local `let` from its initializer when omitted. Do not
perform implicit conversions.

## Consequences

The semantic analyzer needs symbol and type tables, but the bytecode VM remains simple and errors
are caught before execution. More advanced inference and generics are explicitly out of scope.

## How to reverse

Make every `let` require an annotation and remove inference from the grammar/parser. Existing
annotated examples remain valid; only tests for inferred locals need rewriting.

# ADR-0008: Typed AST directly to bytecode in MUST scope

## Context

An extra IR improves extensibility but adds a representation and lowering pass before the team has a
working compiler. The VM instruction set is already a suitable linear representation.

## Options considered

1. AST directly to bytecode.
2. AST to three-address IR to bytecode.
3. Bytecode-only parsing/execution.

## Criteria and scoring

Criteria are schedule fit 4, inspectability 3, optimization headroom 2, correctness risk 3, and
future extensibility 2.

| Option | Schedule | Inspect | Optimize | Risk | Extend | Total |
|---|---:|---:|---:|---:|---:|---:|
| AST → bytecode | 5 | 5 | 2 | 5 | 3 | 56 |
| AST → IR → bytecode | 3 | 5 | 5 | 3 | 5 | 48 |
| Bytecode-only | 4 | 2 | 2 | 2 | 1 | 34 |

## Decision

Use annotated AST directly for the first backend. Keep `src/ir` as a documented STRETCH seam for
constant folding and dead-code removal.

## Consequences

The walking skeleton arrives sooner and fewer invariants cross the critical path. Optimization work
must wait until bytecode generation is tested.

## How to reverse

Implement `IrFunction` lowering from the existing annotated AST, make codegen consume IR, and prove
byte-for-byte semantic equivalence with the existing end-to-end corpus.

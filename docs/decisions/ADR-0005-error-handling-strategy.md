# ADR-0005: Bounded diagnostic collection

## Context

Fail-fast errors are easy to implement but make compiler development and grading slow. The team
needs multiple syntax/type errors from one run without cascading into crashes.

## Options considered

1. Fail fast at the first error.
2. Collect all errors with no cap.
3. Collect errors with a configurable maximum and deterministic sorting.

## Criteria and scoring

Criteria are feedback quality 4, implementation effort 3, determinism 2, robustness 3, and memory
control 2.

| Option | Feedback | Effort | Determinism | Robustness | Memory | Total |
|---|---:|---:|---:|---:|---:|---:|
| Fail fast | 1 | 5 | 5 | 3 | 5 | 43 |
| Unbounded | 5 | 3 | 3 | 2 | 1 | 39 |
| Bounded collection | 5 | 4 | 5 | 5 | 5 | 67 |

## Decision

Collect diagnostics up to 50 by default, configurable with `--max-errors N`. Parser recovery and
semantic error types permit later checks to continue; diagnostics are sorted by source position.

## Consequences

The compiler needs recovery points and error-type propagation, but users receive actionable output
and tests can assert stable ordering.

## How to reverse

Set the collector cap to one and return after the first error; retain diagnostic codes and renderer
format so tests remain mostly compatible.

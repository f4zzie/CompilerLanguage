# ADR-0007: Host-managed values and explicit VM ownership

## Context

The MUST language has primitives and immutable strings; a tracing garbage collector would consume
time without improving the required compiler learning goals.

## Options considered

1. Host-managed C++ values and vectors.
2. Manual arena plus reference-counted runtime heap.
3. Full tracing garbage collector.

## Criteria and scoring

Criteria are implementation effort 4, safety 3, portability 2, educational value 2, and stretch
headroom 2.

| Option | Effort | Safety | Portability | Education | Headroom | Total |
|---|---:|---:|---:|---:|---:|---:|
| Host-managed | 5 | 4 | 5 | 3 | 3 | 51 |
| Arena/refcount | 3 | 3 | 5 | 5 | 4 | 46 |
| GC | 1 | 4 | 4 | 5 | 5 | 36 |

## Decision

Use RAII and `unique_ptr` for compiler-owned structures. Store primitive and string values directly
in `Value`; add shared array storage only for the SHOULD array feature. No user-visible heap or GC is
part of MUST scope.

## Consequences

The implementation is short and safe enough for the assignment, but arrays and future closures
will need a deliberate ownership extension.

## How to reverse

Add a runtime heap abstraction behind `Value` and document allocation/free invariants before adding
closures or cyclic data.

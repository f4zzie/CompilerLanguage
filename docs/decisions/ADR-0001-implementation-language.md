# ADR-0001: C++20 implementation language

## Context

Kite needs a portable compiler and VM that graders can build with common tools. The assignment
allows C++20 and the team wants direct control over bytecode layout without external libraries.

## Options considered

1. C++20 with conservative standard-library usage.
2. C11 with tagged unions, arenas, and hand-written containers.
3. Rust with enums, ownership, and Cargo.

## Criteria and scoring

Scores are 1–5, higher is better; weights are portability 3, learning value 2, AST ergonomics 2,
tool availability 3, and implementation speed 2.

| Option | Portability | Learning | AST | Tools | Speed | Weighted total |
|---|---:|---:|---:|---:|---:|---:|
| C++20 | 5 | 5 | 4 | 5 | 4 | 53 |
| C11 | 5 | 4 | 2 | 5 | 2 | 42 |
| Rust | 4 | 4 | 5 | 3 | 3 | 43 |

## Decision

Use C++20, structs, enums, `std::variant` where useful, `std::unique_ptr`, vectors, strings, and
standard containers. Do not use template metaprogramming or exceptions for normal errors.

## Consequences

The VM can use compact native data structures and common compilers. Ownership discipline must be
reviewed manually, and C++ signed-overflow rules require explicit runtime checks.

## How to reverse

If the assignment mandates pure C, create the C11 branch described in `docs/assumptions.md`, add an
arena allocator, replace AST ownership, and revise estimates before M1.

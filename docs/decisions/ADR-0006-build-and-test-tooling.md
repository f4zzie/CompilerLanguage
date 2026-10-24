# ADR-0006: CMake, CTest, doctest, and CI matrix

## Context

The project must build across Linux, macOS, and Windows with no CLI framework or parser dependency.
The team needs one local command and reproducible CI.

## Options considered

1. CMake + CTest + vendored doctest.
2. Makefiles plus a custom shell test runner.
3. Meson plus Catch2.

## Criteria and scoring

Criteria are OS portability 4, IDE support 2, test integration 3, dependency policy 3, and setup
effort 2.

| Option | Portability | IDE | Tests | Policy | Effort | Total |
|---|---:|---:|---:|---:|---:|---:|
| CMake/CTest/doctest | 5 | 5 | 5 | 5 | 4 | 64 |
| Make/custom | 2 | 2 | 3 | 5 | 3 | 34 |
| Meson/Catch2 | 4 | 4 | 5 | 4 | 3 | 52 |

## Decision

Use CMake 3.20+, presets, CTest, the upstream doctest v2.5.3 single header, shell/PowerShell/batch
helpers, and GitHub Actions for Linux/macOS/Windows.

## Consequences

The repository has a small amount of build configuration but can be opened by common IDEs and run
without installing a package manager. The test framework is a fixed, reviewable upstream dependency.

## How to reverse

Retain CMake as the canonical build and replace only the unit-test framework or CI provider; do not
make shell scripts the only build interface.

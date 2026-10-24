# Contributing to Kite

## Workflow

Kite uses trunk-based development with short-lived branches and pull requests. Branches use
`feat/T###-short-name`, `fix/T###-short-name`, or `docs/T###-short-name`. Keep commits small and
use Conventional Commit prefixes such as `feat:`, `fix:`, `test:`, `docs:`, `refactor:`, and
`chore:`.

## Directory ownership

- Front end: `src/lexer`, `src/parser`, and related unit tests.
- Semantic analysis: `src/sema` and semantic tests.
- Backend/runtime: `src/ir`, `src/codegen`, `src/runtime`, and VM tests.
- Tooling/docs: `src/driver`, `scripts`, `docs`, and examples.

Coordinate interface changes before editing shared headers. Every pull request must include a
task-card reference, tests for changed behavior, a passing Debug build, and an updated document
when a public contract changes.

## Pull request checklist

- [ ] The change is limited to one task card.
- [ ] Tests were added or the reason no test is needed is stated.
- [ ] `cmake --preset linux-debug` and `cmake --build --preset linux-debug` pass.
- [ ] `ctest --test-dir build/linux-debug --output-on-failure` was run.
- [ ] No generated build output or private data is committed.
- [ ] Diagnostics, dump formats, and examples remain deterministic.

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

The definition of done is that the complete default CTest suite passes, including
`walking_skeleton`.

## Pull request checklist

- [ ] The change is limited to one task card.
- [ ] Tests were added or the reason no test is needed is stated.
- [ ] `cmake --preset linux-debug` and `cmake --build --preset linux-debug` pass.
- [ ] The full CTest suite passes, including `walking_skeleton`.
- [ ] No generated build output or private data is committed.
- [ ] Diagnostics, dump formats, and examples remain deterministic.

## Release and CI verification

- GitHub Actions must be green on `ubuntu-latest`, `macos-latest`, and `windows-latest`; do not remove
  a platform from the matrix to hide a failure.
- A fresh clone must be able to run `python3 scripts/self-review.py` without ignored build or package
  artifacts. Generated archives may be validated separately when present.
- If a release-content fix lands after `v1.0`, keep `v1.0` immutable and create an annotated patch
  tag such as `v1.0.1` at the final commit.

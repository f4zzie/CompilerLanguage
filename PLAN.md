# Kite project plan

## Executive summary

Kite is a six-week, three-student compiler assignment targeting a custom stack VM. The MUST tier
contains integers, booleans, strings, unit, mutable `let` bindings, lexical scopes, arithmetic,
comparisons, boolean logic, `if/else`, `while`, functions, recursion, readable diagnostics, dump
commands, and automated end-to-end tests. The SHOULD tier contains floats, arrays, C-style `for`,
the pipe operator `|>`, and improved trace output. A C-transpile backend, REPL, records, closures,
modules, and optimization passes remain stretch or future work.

## Document index

| Document | Purpose |
|---|---|
| `docs/assumptions.md` | Unknowns, defaults, and change procedure |
| `docs/language-spec.md` | Implementable draft grammar and semantics |
| `docs/architecture.md` | Pipeline contracts and runtime design |
| `docs/repo-structure.md` | Every scaffold directory and file |
| `docs/milestones.md` | Dates, exits, dependencies, and team plans |
| `docs/testing-strategy.md` | Test formats and feature matrix |
| `docs/error-and-diagnostics.md` | Error codes and rendering contract |
| `docs/coding-standards.md` | C++ and collaboration conventions |
| `docs/risk-register.md` | Risks, triggers, and mitigations |
| `docs/rubric-traceability.md` | Evidence-backed generic rubric mapping until the real rubric arrives |
| `docs/demo-and-report-plan.md` | Final demo, report, and submission plan |
| `docs/PROGRESS.md` | Resumable implementation status |
| `docs/decisions/` | Architecture decision records |
| `docs/task-cards/` | T001–T029 implementation and delivery tasks |

## Critical path

T001 → T002 → T003 → T004 → T005 → T006/T007 → T008/T009 → T010 → T011 → T012 → T013 → T014 →
T015 → T016/T017 → T018/T019 → T020/T021 → T023/T024/T025 → T027 → T028/T029. Tasks T022, T026,
and most documentation can proceed in parallel after their referenced interfaces exist.

## Milestone summary

| Milestone | Dates | Exit artifact |
|---|---|---|
| M0 Foundations | 2026-10-24–2026-10-25 | Buildable scaffold and frozen contracts |
| M1 Walking skeleton | 2026-10-26–2026-11-01 | `print(1 + 2);` runs through all stages |
| M2 Core language | 2026-11-02–2026-11-08 | Variables, expressions, scopes, branches, loops |
| M3 Functions | 2026-11-09–2026-11-15 | Parameters, returns, recursion, frames |
| M4 Type/error hardening | 2026-11-16–2026-11-18 | Complete diagnostics and negative tests |
| M5 Data and pipe | 2026-11-19–2026-11-23 | Five days for strings, builtins, arrays/floats if green, `|>` SHOULD |
| Buffer | 2026-11-24–2026-11-28 | Five-day slippage reserve, no new MUST features |
| M6 Hardening | 2026-11-29–2026-12-02 | Sanitizers, corpus, performance, docs |
| M7 Release | 2026-12-03–2026-12-05 | Demo, report, clean-machine submission |

M5 has five calendar days. Buffer is five days (about 12%); M6+M7 have seven calendar days (about
17%), satisfying the required final hardening/release reserve. No SHOULD feature may delay the MUST
test and documentation work.

## Self-review checklist

- [x] Unknown parameters have defaults in `docs/assumptions.md`.
- [x] The missing rubric is mapped to generic compiler-course criteria.
- [x] Grammar, token names, AST nodes, and operator table use the same vocabulary.
- [x] Example programs include exact output and compile/runtime failures.
- [x] Pipeline contracts connect source text through VM output.
- [x] Every planned opcode has a stack-effect specification and producer.
- [x] Compile-time error codes have message templates and negative-test names.
- [x] Task cards are split into 2–8 hour units with dependencies and tests.
- [x] The schedule has a five-day M5 and explicit buffer.
- [x] Walking skeleton is the first executable feature and its red test exists.
- [x] The scaffold uses only C++20, CMake, CTest, and upstream doctest v2.5.3.
- [x] No untracked placeholder such as `TBD` or `etc.` is used in normative docs; open questions are explicit.
- [x] Naming is consistent across specification, code, tests, and task cards.
- [x] The pipe operator is in exactly one tier: SHOULD.
- [x] Demo and report plans reference files under `examples/`.

## Final implementation boundary

The MUST pipeline and planned SHOULD features are implemented. The remaining documented boundary is
intentional: the VM is the execution backend, `build -o` emits deterministic disassembly text rather
than a serialized module, and stretch work such as a REPL, records, closures, modules, optimization,
and a C backend is not included. `docs/FINAL-STATUS.md` records the verified commands, evidence gaps,
platform limits, and release decisions.

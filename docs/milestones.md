# Milestones and schedule

The planning baseline is Saturday, October 24, 2026 through Saturday, December 5, 2026. M5 has
five days, the buffer is five days (at least 10%), and hardening/release occupies November 29–
December 5, seven days (at least 15%).
No SHOULD item may consume buffer time while a MUST exit criterion is open.

| Milestone | Dates | Scope | Objective exit criteria | Demo artifact | Cut if behind |
|---|---|---|---|---|---|
| M0 Foundations | Oct 24–25 | Docs, ADRs, CMake, diagnostics scaffold | Debug configure/build succeeds; docs and task cards exist; CTest discovers tests | `kite --help` | All stretch planning detail beyond required contracts |
| M1 Walking skeleton | Oct 26–Nov 1 | Lexer, parser, sema, bytecode, VM, CLI path | `print(1 + 2);` exits 0 and prints `3`; tokens/AST/disasm commands work | First end-to-end run | Floats, arrays, pipe |
| M2 Core language | Nov 2–8 | Variables, scopes, expressions, branches, while | Positive and negative tests cover every MUST expression/control construct | `examples/loops.kite` | For loop and arrays |
| M3 Functions | Nov 9–15 | Functions, parameters, returns, calls, recursion, frames | Factorial and Fibonacci examples pass; wrong arity/type tests pass | `examples/recursion.kite` | Any new builtin beyond listed set |
| M4 Type/error hardening | Nov 16–18 | Complete diagnostics, parser recovery, deterministic dumps | Each compile error code has a negative test and caret output | Clean multi-error diagnostic demo | Pretty colors, optional notes |
| M5 Data and pipe | Nov 19–23 | Five days for strings, builtins, SHOULD arrays/floats/for, pipe | Strings and builtins pass; SHOULD features only merge if all MUST tests remain green | `examples/pipe.kite` | Arrays, floats, then pipe in that order |
| Buffer | Nov 24–28 | Five-day slippage and integration repairs | No unresolved MUST blocker; scope freeze at Nov 28 | Re-run full corpus | All remaining SHOULD work |
| M6 Hardening | Nov 29–Dec 2 | Sanitizers, fuzz smoke, performance, docs | Debug/Release/sanitizer checks recorded; 1,000-line smoke target measured | Clean build report | Fuzzing if it threatens release |
| M7 Release | Dec 3–5 | Report, demo, packaging, clean-machine rehearsal | Submission archive unpacks and builds; demo follows script; report references evidence | Final presentation run | Cosmetic README changes only |

## Dependency graph

```mermaid
graph TD
  T001[T001 foundations] --> T002[T002 spec and ADRs]
  T002 --> T003[T003 diagnostics]
  T003 --> T004[T004 lexer]
  T003 --> T005[T005 AST]
  T004 --> T006[T006 expression parser]
  T005 --> T006
  T006 --> T007[T007 statements and recovery]
  T007 --> T008[T008 symbols]
  T008 --> T009[T009 type checker]
  T007 --> T010[T010 bytecode model]
  T009 --> T011[T011 code generator]
  T010 --> T011
  T010 --> T012[T012 VM]
  T011 --> T013[T013 driver]
  T012 --> T013
  T013 --> T014[T014 walking skeleton]
  T014 --> T015[T015 variables and scopes]
  T015 --> T016[T016 control flow]
  T014 --> T017[T017 functions and frames]
  T017 --> T018[T018 recursion and returns]
  T015 --> T019[T019 strings and builtins]
  T018 --> T020[T020 pipe SHOULD]
  T019 --> T021[T021 arrays/floats/for SHOULD]
  T014 --> T022[T022 golden tests]
  T014 --> T023[T023 end-to-end runner]
  T009 --> T024[T024 negative tests]
  T022 --> T025[T025 fuzz and sanitizer]
  T023 --> T026[T026 README and docs]
  T025 --> T027[T027 demo and report]
  T026 --> T027
  T027 --> T028[T028 submission package]
```

## Staffing plans

### One student

Follow the critical path in order. Spend the first four weeks only on MUST work. Start documentation
and tests in parallel as interfaces stabilize. Cut arrays, floats, pipe, fuzzing, and cosmetic CLI
features first. The MUST implementation budget is capped at roughly 40–44 hours.

### Two students

Student A owns lexer/parser/sema. Student B owns bytecode/VM/driver. Both review diagnostics and
tests. T019 and T022–T026 can proceed in parallel once T014 defines the pipeline contract.

### Three students

Student A owns lexer/parser and AST. Student B owns symbols/types/diagnostics. Student C owns
bytecode/VM/driver. All three agree on `Token`, AST, `Diagnostic`, and `BytecodeModule` contracts in
M0. A shared integration branch is updated at each milestone; no one works directly on another
owner's internal files without a review.

## Weekly cadence

1. Monday: 20-minute planning; select cards whose dependencies are green.
2. Midweek: each owner posts one failing or passing test and one risk update.
3. Friday: runnable demo, even if the demo is a dump command or diagnostic case.
4. Saturday: definition-of-done review; merge only code with tests and updated docs.
5. If behind: freeze new features, repair MUST path, cut arrays, floats, pipe, fuzzing, then cosmetic output.

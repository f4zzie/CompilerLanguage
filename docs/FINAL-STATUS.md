# Kite final status

## Overall result

The MUST compiler pipeline and all four planned SHOULD language features—floats, arrays, C-style
`for`, and `|>`—are implemented on Linux. The
repository is clean at the release boundary and the Debug, Release, and ASan/UBSan CTest suites are
green. STRETCH work (REPL, records,
closures, modules, optimizer, and C backend) was not started.

## Task status

| Task | Status | Merge commit |
|---|---|---|
| T001 | done | 86d471d |
| T002 | done | 2aa662f |
| T003 | done | 86f74b3 |
| T004 | done | d341459 |
| T005 | done | e313b89 |
| T006 | done | 7427231 |
| T007 | done | c9a56d6 |
| T008 | done | f434abe |
| T009 | done | f4261de |
| T010 | done | 81b2201 |
| T011 | done | 45a93fa |
| T012 | done | 5f0cb12 |
| T013 | done | b71424b |
| T014 | done | b32ea55 |
| T015 | done | f4a306b |
| T016 | done | b288b1b |
| T017 | done | 4af1c1d |
| T018 | done | 32a9140 |
| T019 | done | c1c4aec |
| T020 | done | 2eca8ce |
| T021 | done | adaf3a6 |
| T022 | done | 2346692 |
| T023 | done | bc27066 |
| T024 | done | 1ad1c55 |
| T025 | done | 88b34ed |
| T026 | done | 868ea6a |
| T027 | done | f42a591 |
| T028 | done | 6721fd8 |
| T029 | done | f18d961 |

## SHOULD feature status

| Feature | Status | Evidence |
|---|---|---|
| Floats | implemented | `examples/float.kite`, `tests/unit/arrays_for_test.cpp` |
| Arrays | implemented | `examples/arrays-for.kite`, `tests/unit/arrays_for_test.cpp` |
| C-style `for` | implemented | `examples/arrays-for.kite`, `tests/unit/arrays_for_test.cpp` |
| `|>` pipe | implemented | `examples/pipe.kite`, `tests/unit/pipe_test.cpp` |

## Verified commands

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --test-dir build/linux-debug --output-on-failure
./scripts/test-all
cmake --preset linux-asan
cmake --build --preset linux-asan
ctest --test-dir build/linux-asan --output-on-failure
cmake --preset release
cmake --build --preset release
ctest --test-dir build/release --output-on-failure
python3 scripts/check-examples.py build/linux-debug/kite examples
python3 scripts/run-corpus.py build/linux-debug/kite tests/programs
python3 scripts/run-errors.py build/linux-debug/kite tests/errors
./scripts/package-submission.sh
python3 scripts/self-review.py
```

Observed on October 25, 2026: Debug CTest 7/7, Release CTest 7/7, and ASan/UBSan CTest 7/7 passed;
each suite includes the unit tests, walking skeleton, fuzz smoke, examples corpus, and three golden
tests. The automated negative corpus contains seven fixtures with exit-code and stderr assertions.
The package manifest check reported 280 archive entries before the final four-fixture expansion. A
clean extraction Debug build and
CTest also passed, and `scripts/self-review.py` passed after the final documentation checks.

## Performance evidence

Method: every reported benchmark timing is the median of five consecutive wall-clock
runs of `./build/release/kite run benchmarks/fib30.kite`; each run must exit successfully and print
exactly `832040`.
- The deterministic fuzz smoke runs 1,000 arbitrary byte inputs and passes in Debug and ASan builds.
- The gprof timings below are single instrumented profiling samples, retained only to identify
  hotspots and not used as benchmark timing results.
- Performance re-measurement on October 25, 2026 used `/usr/bin/gprof` with a Release `-pg` build and
  `benchmarks/fib30.kite`. Before the change, `Vm::run` accounted for 62.82% of sampled self time,
  with `Span` copies and the `std::variant` equality visitor each at 11.54%; the instrumented run
  took 3.404 seconds. The safe VM change keeps the instruction in a const reference and moves values
  out of the operand stack when popping. Afterward, the same instrumented run took 2.613 seconds;
  `Vm::run` was 43.84% of sampled self time. The five-run native Release median was 3.188692
  seconds before the change and 1.461218 seconds at the final verification point, a 54.18% reduction,
  with output still exactly `832040`.
- Release archive SHA-256 is recorded in the generated, ignored sidecar `submission.zip.sha256`.
  The package script creates the archive, validates its manifest, and writes this sidecar.
- The package script explicitly excludes build output, `.git` data, logs, profiling output, caches,
  Python bytecode, the archive itself, and its checksum sidecar. Final archive verification reports no
  forbidden entries and a clean extraction Debug build.

## Autonomous decisions and contract changes

1. Replaced the local doctest stand-in with upstream doctest v2.5.3.
2. Made T014 a CTest process test and changed `examples/hello.kite` to the arithmetic walking source;
   moved the greeting to `examples/hello-world.kite`.
3. Added `ArrayExpr`, `AssignmentExpr`, `MAKE_ARRAY`, and `INDEX` to complete the implementation of
   already-specified SHOULD syntax/runtime behavior.
4. Added deterministic parser progress guards after fuzzing found a binary/NUL lexer hang.
5. `build -o` currently writes deterministic disassembly text rather than a serialized executable
   bytecode module; this is a documented limitation and should be reviewed before submission.

## Cuts and limitations

- No REPL, records, closures, modules/imports, optimizer, C backend, or full debugger.
- `--color` is parsed but rendering remains plain text.
- The built module is in-memory; `build -o` is a disassembly artifact.
- Runtime arrays support construction/indexing/length but not mutation.
- The semantic checker has a compact builtin model rather than a fully generalized function-type system.

## Needs a human

- Verify macOS and Windows/MSVC builds, PowerShell scripts, and `.bat` scripts.
- Run the real CI workflow on GitHub.
- Supply the official grading rubric and replace generic traceability rows.
- Review the report and demo materials in the team's own words.
- Decide whether the text `build -o` artifact satisfies the assignment or must become serialized bytecode.

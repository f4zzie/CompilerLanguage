# Kite final status

## Overall result

The MUST compiler pipeline and all four planned SHOULD language features—floats, arrays, C-style
`for`, and `|>`—are implemented. Parser input is bounded at 200 levels by default with configurable
`P004` recovery, and `build`/`exec` use validated versioned `.kbc` files. The Debug, Release, and
ASan/UBSan CTest suites are green locally. STRETCH work (REPL, records, closures, modules, optimizer,
and C backend) was not started.

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
| Parser depth hardening | done | 082612c |
| Versioned bytecode files | done | aea1fc6 |
| Truthful CI matrix | done | 181c245 |
| Current presentation docs | done | 6dfae9f |
| License status note | done | f9b1115 |

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
cmake --preset linux-asan
cmake --build --preset linux-asan
ctest --test-dir build/linux-asan --output-on-failure
cmake --preset release
cmake --build --preset release
ctest --test-dir build/release --output-on-failure
python3 scripts/check-examples.py build/linux-debug/kite examples
python3 scripts/run-errors.py build/linux-debug/kite tests/errors
python3 scripts/check-bytecode.py build/linux-debug/kite examples
```

Observed on November 3, 2026: Debug, Release, and ASan/UBSan CTest each passed 8/8. The Debug unit
runner passed 61 cases and 202 assertions; the negative corpus passed 10/10; the examples corpus
passed 12/12; and the bytecode checker passed 14/14, including truncated and corrupt files.

## Post-release verification

On October 26, 2026, a fresh clone from `origin/main` was verified independently. Debug, Release, and
Linux ASan/UBSan configure/build/CTest all passed; `report/demo-smoke.sh` passed; all Getting started
and Compiler commands in `README.md` were run successfully; and the presentation commands produced
the outputs recorded in `docs/STUDY-GUIDE.md` and `docs/DEMO-SCRIPT.md`. The first fresh-clone run
exposed a real tooling issue: `scripts/self-review.py` required the ignored checksum sidecar even
though a clone does not contain generated release artifacts. The root fix changed self-review to
require the tracked package script instead; the fresh-clone self-review then passed.

The latest accessible GitHub Actions run before this matrix change was `40220486356`, completed on
October 25, 2026 with all previous release-matrix jobs green:

| Job | Result |
|---|---|
| previous Ubuntu release job | passed |
| previous macOS release job | passed |
| previous Windows release job | passed |

The revised workflow now defines five jobs—Linux Debug, Linux ASan/UBSan, Release Ubuntu, Release
macOS, and Release Windows. Its post-push results were not available when this status file was written.

The Windows failure in the preceding runs was a MinGW `-Werror=maybe-uninitialized` warning through
`std::variant` temporaries in `src/codegen/code_generator.cpp`. Typed `add_constant` overloads and an
explicit integer conversion fixed the root cause without weakening warnings or tests. No CI matrix
job was removed. Native macOS/Windows execution outside GitHub-hosted runners remains **not verified**
in this Linux environment.

The post-release verification, CI repair, presentation materials, and hardening changes are prepared
for the annotated `v1.0.1` tag. The original published `v1.0` tag remains immutable at its original
commit.

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
5. `build -o` now writes a versioned binary `.kbc` module; `exec` validates and runs it independently
   of the source file.

## Cuts and limitations

- No REPL, records, closures, modules/imports, optimizer, C backend, or full debugger.
- `--color` is parsed but rendering remains plain text.
- Runtime arrays support construction/indexing/length but not mutation.
- The semantic checker has a compact builtin model rather than a fully generalized function-type system.

## Needs a human

- The `LICENSE` file is a placeholder in this public repository; a human must choose and add the
  appropriate license before publication or submission.
- Verify macOS and Windows/MSVC builds, PowerShell scripts, and `.bat` scripts.
- Run the real CI workflow on GitHub.
- Supply the official grading rubric and replace generic traceability rows.
- Review the report and demo materials in the team's own words.
- Review the new study guide, exercises, oral-exam questions, and demo script in the student's own words.

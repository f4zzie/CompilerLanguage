# Kite final status

## Overall result

The MUST compiler pipeline and all planned SHOULD language features are implemented on Linux. The
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
| T029 | done | release branch |

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
tests. The package manifest check reported 272 archive entries. A clean extraction Debug build and
CTest also passed, and `scripts/self-review.py` passed after the final documentation checks.

## Performance evidence

- `fib(30)` in Release produced `832040` in 5.243115 seconds on the Linux host used for verification.
- A generated 1,001-line Debug program produced `999` in 0.077898 seconds.
- The deterministic fuzz smoke runs 1,000 arbitrary byte inputs and passes in Debug and ASan builds.
- Release archive SHA-256 is recorded in the generated, ignored sidecar `submission.zip.sha256`.
  The package script creates the archive, validates its manifest, and writes this sidecar.

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
- Review the 11.479-second Release `fib(30)` result against the course performance expectation.

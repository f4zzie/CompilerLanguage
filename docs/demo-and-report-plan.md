# Demo and report plan

## Demo sequence (DRAFT)

The final demonstration should take 8–10 minutes and use a clean Debug build.

1. Show the repository and run `cmake --preset linux-debug` and `cmake --build --preset linux-debug`.
2. Run `examples/hello.kite` to establish the arithmetic walking skeleton, then run
   `examples/hello-world.kite` for the greeting demo.
3. Run `examples/arithmetic.kite` and `examples/scope.kite` to show values and lexical binding.
4. Run `examples/conditionals.kite` and `examples/loops.kite` for control flow.
5. Run `examples/recursion.kite` to demonstrate functions, returns, and call frames.
6. Run `kite tokens`, `kite ast`, and `kite disasm` on the recursion example to expose each stage.
7. Run `examples/pipe.kite --trace` to demonstrate the SHOULD distinctive feature and VM stack.
8. Run `tests/errors/type-mismatch.kite` and `tests/errors/division-by-zero.kite` to show compile-
   time and runtime diagnostics with exit codes.
9. Run CTest and show the final submission archive contents.

The runnable smoke script is `report/demo-smoke.sh`; its PowerShell counterpart is
`report/demo-smoke.ps1`. Linux execution of the shell script is the verified path; PowerShell and
Windows execution remain unverified.

If a SHOULD feature is cut, replace the pipe demo with `kite disasm` plus the diagnostic demo; the
MUST presentation remains complete.

## Report outline

1. Problem statement and design goals.
2. Language philosophy and complete feature table.
3. Lexical and syntactic design with representative grammar.
4. Compiler architecture and stage contracts.
5. Semantic analysis, scopes, and typing rules.
6. Bytecode instruction set and VM call-frame design.
7. Diagnostics and error recovery.
8. Testing strategy, test matrix, and measured results.
9. Team division, milestone history, and scope cuts.
10. Limitations, risks, and future work.
11. Conclusion and lessons learned.

## Evidence to capture

- Screenshot of `kite tokens`, `kite ast`, and `kite disasm` output.
- Screenshot of a caret diagnostic and a runtime error.
- Screenshot or terminal capture of CTest and sanitizer results.
- Table of positive, negative, and golden test counts.
- Clean-machine build commands and exact compiler version.

## Submission package

The release archive contains source, `CMakeLists.txt`, presets, scripts, docs, tests, examples,
third-party doctest header, and the report PDF or Markdown required by the course. It excludes
`build/`, editor settings, credentials, and local logs. `scripts/package-submission.sh` or its
PowerShell equivalent will create the archive after T028.

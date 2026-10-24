# Repository structure

```text
.
├── .github/workflows/ci.yml       Cross-platform CI workflow.
├── .clang-format                  Shared C++ formatting rules.
├── .gitignore                     Build and editor exclusions.
├── CMakeLists.txt                 Main CMake target graph and warning policy.
├── CMakePresets.json              Debug, sanitizer, and Release presets.
├── CONTRIBUTING.md                Branching, ownership, review, and checks.
├── LICENSE                        License placeholder for course approval.
├── PLAN.md                        Master plan and completed self-review checklist.
├── README.md                      User-facing project overview and quick start.
├── cmake/README.md                Notes for future reusable CMake modules.
├── docs/                          Specification, architecture, process, and task cards.
│   ├── PROGRESS.md                Resumable task status and next step.
│   ├── assumptions.md             Defaults for missing assignment information.
│   ├── architecture.md            Pipeline contracts, VM, opcode table, extension recipes.
│   ├── coding-standards.md        Source and collaboration conventions.
│   ├── demo-and-report-plan.md    Presentation, report, and submission plan.
│   ├── error-and-diagnostics.md   Error taxonomy and renderer contract.
│   ├── language-spec.md           Draft Kite grammar and semantics.
│   ├── milestones.md              Calendar, dependencies, staffing, and cadence.
│   ├── repo-structure.md          This file.
│   ├── risk-register.md            Risks, triggers, and mitigations.
│   ├── rubric-traceability.md     Generic rubric mapping until the rubric is supplied.
│   ├── testing-strategy.md        Test pyramid and exact test file formats.
│   ├── decisions/                 Architecture decision records.
│   └── task-cards/                T001–T029 implementation and delivery cards.
├── examples/                      Readable demo programs.
│   ├── hello.kite                 Smallest successful output program.
│   ├── arithmetic.kite            Arithmetic and local binding example.
│   ├── scope.kite                 Nested lexical scope example.
│   ├── conditionals.kite           Boolean condition example.
│   ├── loops.kite                 While-loop example.
│   ├── recursion.kite             Factorial and Fibonacci recursion.
│   ├── functions.kite              Multiple-parameter call example.
│   ├── strings.kite                String concatenation and length.
│   ├── pipe.kite                   SHOULD pipe demonstration.
│   ├── arrays-for.kite             SHOULD arrays and for loop.
│   └── float.kite                  SHOULD float example.
├── report/                         Final report outline and evidence checklist.
├── scripts/                       Cross-platform helper scripts.
│   ├── build.sh                   Configure and build Linux Debug.
│   ├── build.ps1                  Configure and build PowerShell Debug.
│   ├── build.bat                  Configure and build Windows Debug from cmd.exe.
│   ├── test-all                   Build and run CTest plus corpus runner.
│   ├── test-all.ps1               PowerShell equivalent.
│   ├── test-all.bat               cmd.exe equivalent.
│   ├── format.sh                  Run clang-format over C++ sources.
│   ├── format.ps1                 PowerShell formatter wrapper.
│   ├── run-examples.sh            Run showcase programs.
│   └── run-examples.ps1           PowerShell showcase runner.
├── src/                           C++20 compiler implementation.
│   ├── common/config.hpp          Single language-name and extension configuration constants.
│   ├── common/source.hpp          Source locations and spans.
│   ├── diagnostics/               Diagnostic collection, rendering, source map.
│   ├── lexer/                     Token kinds, scanner, and token dump.
│   ├── parser/                    AST, AST printer, grammar parser, precedence.
│   ├── sema/                      Types, symbols, scopes, checker.
│   ├── ir/                        Optional future linear IR.
│   ├── codegen/                   Bytecode format, disassembler, emitter.
│   ├── runtime/                   Values, builtins, VM, runtime errors.
│   └── driver/                    CLI and pipeline orchestration.
├── tests/                         Automated verification.
│   ├── test_main.cpp              doctest entry point.
│   ├── spec/                      Contract review checklists.
│   ├── unit/                      Per-stage unit tests.
│   ├── golden/                    Token, AST, and disassembly fixtures.
│   ├── programs/                  Positive end-to-end corpus.
│   └── errors/                    Negative compile/runtime corpus.
└── third_party/doctest/           One vendored single-header test framework.
```

The ownership rule is directory-based: a task touching one stage should not edit another stage's
internal files unless it changes a documented public contract. Shared contract changes are reviewed
by all affected owners.

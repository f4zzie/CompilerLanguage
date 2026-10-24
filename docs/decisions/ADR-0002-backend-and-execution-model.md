# ADR-0002: Custom bytecode and stack VM

## Context

The assignment says compiler but does not require native machine code. The team needs an executable
artifact early, a backend that is inspectable in a demo, and no external toolchain dependency.

## Options considered

1. Tree-walking interpreter.
2. Custom stack bytecode and VM.
3. Transpile to C.
4. LLVM, JVM bytecode, or WASM backend.

## Criteria and scoring

Score 1–5; criteria are compiler educational value 3, early end-to-end speed 3, demo impact 2,
debuggability 2, performance 1, and toolchain risk 3.

| Option | Education | Early | Demo | Debug | Perf | Risk | Weighted total |
|---|---:|---:|---:|---:|---:|---:|---:|
| Tree walker | 3 | 5 | 2 | 5 | 2 | 5 | 48 |
| Bytecode VM | 5 | 4 | 5 | 4 | 4 | 4 | 62 |
| C transpiler | 4 | 3 | 4 | 3 | 4 | 3 | 49 |
| LLVM/JVM/WASM | 5 | 1 | 4 | 2 | 5 | 1 | 38 |

## Decision

Compile annotated AST directly to custom stack bytecode and execute it in a C++ VM. A C-transpile
backend is a stretch contingency and becomes MUST only if the rubric requires native output.

## Consequences

The team owns a small instruction set, call frames, and runtime errors, but gains deterministic
disassembly, stack tracing, and no dependency beyond C++.

## How to reverse

Keep the lexer/parser/sema contracts. Add `src/target_c`, emit a runtime header plus C source, and
reuse the same end-to-end corpus. Do not remove the VM until the native path passes the same tests.

# Kite 7–10 minute demonstration script

This script is designed for a student who needs to explain the implementation, not merely run it.
Run from the repository root. The timings below are real Linux measurements from October 26, 2026
using the Debug build and a freshly rebuilt Release binary where noted. They are guidance, not a
performance guarantee on another machine.

## Before the audience arrives

Run once:

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
```

Observed no-op verification timings on the current machine:

| Command | Time | Result |
|---|---:|---|
| `cmake --preset linux-debug` | 0.058186 s | exit 0 |
| `cmake --build --preset linux-debug` | 0.027540 s | exit 0 |

If the build directory came from another toolchain and CMake rejects its cached Ninja path, use a
new build directory or reconfigure with `cmake -S . -B build/demo -G Ninja -DCMAKE_BUILD_TYPE=Debug
-DKITE_BUILD_TESTS=ON`; do not edit source code to work around a local cache.

## 0:00–0:45 — State the thesis

Say:

> Kite is a small statically typed language with a complete inspectable pipeline: source, tokens, AST,
> semantic checks, bytecode, and a stack VM. I will show one artifact from each stage and then two
> failures that prove the error contracts are real.

## 0:45–1:30 — Run a program

Command:

```sh
./build/linux-debug/kite run examples/hello.kite
```

Observed time: **0.019524 s**. Expected output:

```text
3
```

Say: “This is the walking-skeleton behavior: `print(1 + 2);` reaches the VM and produces program
output on stdout.”

## 1:30–2:15 — Show tokens and AST

Commands:

```sh
./build/linux-debug/kite tokens examples/hello.kite
./build/linux-debug/kite ast examples/hello.kite
```

Observed times: **0.018408 s** and **0.021671 s**. The key token output is:

```text
1:1 identifier `print`
1:7 integer `1`
1:9 + `+`
1:11 integer `2`
2:1 eof ``
```

The AST contains:

```text
CallExpr#2 callee=print
  BinaryExpr#5 op=+
```

Say: “The lexer preserves source positions. The parser turns the expression into a tree; Pratt
precedence keeps the `+` expression inside the call.”

## 2:15–2:50 — Show bytecode

Command:

```sh
./build/linux-debug/kite disasm examples/hello.kite
```

Observed time: **0.019274 s**. Output:

```text
0	CONSTANT	0
1	CONSTANT	1
2	ADD	0
3	CALL	1
4	POP	0
5	HALT	0
```

Say: “The backend turns the AST into a deterministic stack program. Constants are pushed, `ADD`
combines them, `CALL` invokes `print`, and `HALT` ends the synthetic main function.”

## 2:50–3:40 — Trace the VM

Command:

```sh
./build/linux-debug/kite run examples/pipe.kite --trace
```

Observed time: **0.020193 s**. Stdout is:

```text
10
```

The first trace lines on stderr are:

```text
0 CONSTANT []
1 CALL [5]
5 LOAD_LOCAL []
6 CONSTANT [5]
7 MULTIPLY [5, 2]
8 RETURN [10]
```

Say: “The pipe expression is not magic at runtime. It evaluates the value, calls `double`, returns
`10`, and then calls `print`. Trace goes to stderr so stdout remains usable.”

## 3:40–4:20 — Show implemented SHOULD features

Commands:

```sh
./build/linux-debug/kite run examples/float.kite
./build/linux-debug/kite run examples/arrays-for.kite
./build/linux-debug/kite run examples/pipe.kite
```

Observed times: **0.005805 s**, **0.007232 s**, and **0.020193 s**. Expected outputs:

```text
3.5
6
10
```

Say: “Floats use numeric VM values, arrays use construction/indexing/`len`, the C-style `for` has
initialization/condition/update, and pipe syntax lowers to a call. Array mutation is intentionally
outside the current contract.”

## 4:20–5:00 — Prove recursion

Command:

```sh
./build/linux-debug/kite run examples/recursion.kite
```

Observed time: **0.009389 s**. Expected output:

```text
120
8
```

Say: “These are `fact(5)` and `fib(6)`. The important evidence is not the arithmetic; it is that
recursive calls get separate frames and return to the correct instruction.”

## 5:00–5:45 — Show a compile-time failure

Command:

```sh
./build/linux-debug/kite run tests/errors/type-mismatch.kite
```

Observed time: **0.016722 s**, exit status **1**. Expected stderr:

```text
tests/errors/type-mismatch.kite:1:14: T001 type mismatch: expected int, found bool
let x: int = true;
             ^~~~
```

Say: “The semantic analyzer blocks code generation, gives a stable code and span, and the renderer
adds the source line and caret.”

## 5:45–6:20 — Show a runtime failure

Command:

```sh
./build/linux-debug/kite run tests/errors/division-by-zero.kite
```

Observed time: **0.015214 s**, exit status **3**. Expected stderr:

```text
runtime[R001]: division by zero
```

Say: “This source is statically valid. The VM detects the invalid divisor during execution, so the
failure is a runtime error rather than a compile error.”

## 6:20–7:10 — Close with evidence

Commands:

```sh
ctest --test-dir build/linux-debug --output-on-failure
python3 scripts/run-errors.py build/linux-debug/kite tests/errors
```

The verified result is **7/7 CTest tests passed** and **7/7 negative fixtures passed**. Mention that
the same repository also has Release and Linux ASan/UBSan CTest evidence plus green GitHub Actions on
Ubuntu, macOS, and Windows.

For the optional performance close, use the documented five-run median method:

```sh
cmake --preset release
cmake --build --preset release
./build/release/kite run benchmarks/fib30.kite
```

The current verification measured five samples of `4.682137`, `3.546252`, `4.120789`, `3.455777`,
and `4.862819` seconds; the median was **4.120789 s**, with output `832040`. Earlier release evidence
used a different compiler/toolchain and is kept separately in `docs/FINAL-STATUS.md`.

## Live fallback plan

1. **Build failure:** stop and show `cmake --preset linux-debug` output; switch to the already-built
   binary only if the failure is clearly environmental and the binary timestamp is known.
2. **Unexpected demo output:** run the same source with `tokens`, `ast`, or `disasm` to locate the
   failing stage instead of guessing. The study guide gives the expected artifacts.
3. **Compile diagnostic failure:** use `tests/errors/type-mismatch.kite`, whose expected exit status
   is `1`; do not edit the fixture live.
4. **Runtime diagnostic failure:** use `tests/errors/division-by-zero.kite`, whose expected exit
   status is `3`; explain the distinction even if the wording differs.
5. **Terminal/display problem:** present the checked-in output blocks from `docs/STUDY-GUIDE.md` and
   explain which source file would produce each artifact. Do not claim that an unrun command passed.

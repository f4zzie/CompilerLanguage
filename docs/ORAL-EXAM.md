# Kite oral-exam questions and answers

The answers below are short defense answers grounded in the current repository. When a question asks
about an unimplemented feature, the answer says so rather than implying future work is present.

## Pipeline and architecture

1. **What does Kite compile?** — `.kite` source is lexed into tokens, parsed into an AST, checked by
   the semantic analyzer, lowered into stack bytecode, and executed by the VM. See `README.md` and
   `docs/architecture.md`.

2. **Why use bytecode instead of interpreting the AST directly?** — Bytecode gives the backend a
   deterministic, inspectable artifact and keeps execution separate from source-tree shape. The
   instruction contract is in `src/codegen/bytecode.hpp`.

3. **Why a stack VM?** — A stack machine keeps the first backend small: expressions naturally push
   values and operators consume operands. Stack effects are documented in `docs/architecture.md` and
   enforced by `src/runtime/vm.cpp`.

4. **Why not LLVM?** — The project intentionally avoids large dependencies and keeps every stage
   inspectable for learning. `README.md` describes the conservative C++20/standard-library choice.

5. **Where is the pipeline orchestrated?** — `src/driver/pipeline.cpp` runs source manager, lexer,
   parser, semantic analysis, code generation, and optional VM execution.

6. **What is the optional IR for?** — `src/ir/` reserves a future linear representation for
   optimization; the current MUST pipeline lowers the annotated AST directly to bytecode. See the
   first section of `docs/architecture.md`.

7. **What is the most important invariant between stages?** — Code generation runs only when there
   are no error diagnostics, and every emitted instruction has a source span. See the semantic and
   code-generation contracts in `docs/architecture.md`.

8. **How do you inspect each stage?** — Use `tokens`, `ast`, and `disasm`; use `run --trace` for VM
   execution. The CLI is implemented in `src/driver/main.cpp` and `src/driver/cli.cpp`.

## Lexer and parser

9. **Why recursive descent plus Pratt parsing?** — Recursive descent makes statement grammar and
   recovery readable; Pratt parsing handles precedence and associativity without a large expression
   switch. See `src/parser/parser.cpp` and `src/parser/precedence.hpp`.

10. **What does the lexer do with an illegal character?** — It consumes one byte and emits `L001`,
    preserving progress instead of hanging. See `src/lexer/lexer.cpp`.

11. **How are comments handled?** — Line comments stop at newline; block comments scan until `*/`
    or report `L002` at EOF. See `skip_whitespace_and_comments` in `src/lexer/lexer.cpp`.

12. **How are strings represented?** — The lexer decodes supported escapes into the token lexeme and
    reports `L004` for invalid escapes. See `scan_string` in `src/lexer/lexer.cpp`.

13. **How does the parser avoid an infinite loop after an error?** — It records the token index before
    a statement; if recovery made no progress, it advances one token. See `Parser::parse` and
    `parse_block` in `src/parser/parser.cpp`.

14. **What is panic-mode recovery?** — On a recoverable parse failure, the parser skips to a
    semicolon, closing brace, EOF, or a statement/declaration keyword. The synchronization routine is
    `Parser::synchronize_statement`.

15. **How are spans represented?** — `Span` contains one-based source locations with byte offsets;
    the end is exclusive. The definitions are in `src/common/source.hpp`.

16. **Why are AST nodes owned by `std::unique_ptr`?** — Ownership follows the tree, avoiding manual
    deletion and making partially built nodes explicit. The hierarchy is in `src/parser/ast.hpp`.

17. **How is pipe syntax represented?** — `PipeExpr` stores the left value and a function name; it
    is not a special runtime calling convention. See `src/parser/ast.hpp` and `parser.cpp`.

18. **What happens if `|>` targets a missing function?** — Semantic analysis reports `S002`; code
    generation does not run for the invalid program. See `src/sema/sema.cpp`.

## Static semantics

19. **Why static typing?** — It catches mismatched values before execution and makes bytecode/runtime
    contracts explicit. The type rules are implemented in `src/sema/sema.cpp`.

20. **What types exist?** — `int`, `float`, `bool`, `string`, `unit`, arrays, and an internal error
    type. See `TypeKind` in `src/parser/ast.hpp`.

21. **Why have an `Error` type?** — After one error, analysis can continue checking nearby expressions
    without producing cascades for every dependent node. See `is_error` and `check_expression` in
    `src/sema/sema.cpp`.

22. **How are scopes implemented?** — `SymbolTable` maintains a stack of lexical scopes; lookup
    searches the current scope and parents. See `src/sema/symbol.hpp` and `symbol.cpp`.

23. **How does recursion work before a function body is checked?** — Function signatures are
    predeclared in a first pass, so recursive and forward calls can resolve. See the first loop in
    `SemanticAnalyzer::analyze`.

24. **What is `S001`?** — Duplicate declaration in one scope. The analyzer rejects the insertion and
    reports it with the declaration span.

25. **What is `S002`?** — An unresolved name or call target. It is reported in `src/sema/sema.cpp`
    and covered by `tests/errors/unresolved-name.src`.

26. **How are function arguments checked?** — The analyzer compares argument count first, then checks
    each argument against the corresponding parameter type. See the call branch in `check_expression`.

27. **Why are arrays homogeneous?** — A compact static type model keeps indexing and VM values
    predictable. Mixed element types report `T001` in the array branch of `src/sema/sema.cpp`.

28. **What is intentionally not supported for arrays?** — Construction, indexing, and `len` are
    supported; mutation is outside the current contract. This limitation is recorded in
    `docs/FINAL-STATUS.md`.

29. **How are boolean `and` and `or` implemented?** — The code generator emits conditional jumps so
    the right operand is evaluated only when needed. See the short-circuit branch in
    `src/codegen/code_generator.cpp`.

30. **What does panic-mode recovery trade away?** — It improves progress and multi-error reporting,
    but a recovered AST may omit the malformed construct. Code generation is blocked whenever errors
    remain, so invalid partial trees do not execute.

## Bytecode and VM

31. **What is the operand-stack convention?** — Binary operations see left then right values on the
    stack and replace them with one result. The table in `docs/architecture.md` lists each stack effect.

32. **How are locals stored?** — Function frames own a vector of local values, and bytecode addresses
    them by frame-relative slot. See `Frame` and `LoadLocal`/`StoreLocal` in `src/runtime/vm.cpp`.

33. **How are globals stored?** — The module has a global vector; top-level `let` statements use
    `LoadGlobal` and `StoreGlobal`. See `BytecodeModule` and `Emitter::emit_statement`.

34. **How does a call frame return?** — A frame stores function index, instruction pointer, and return
    instruction; `RETURN` restores the caller frame and result. See `Frame` in `src/runtime/vm.cpp`.

35. **How is call-stack overflow handled?** — The VM caps frames at 1024 and returns `R004` instead
    of exhausting the host stack. The source-level fixture is `tests/errors/call-stack-overflow.kite`.

36. **What is `R006`?** — It is the malformed-bytecode/internal runtime guard for impossible VM states
    such as invalid constant indexes, stack underflow, or bad jump targets. Unit coverage is in
    `tests/unit/vm_test.cpp`.

37. **Why does the VM move values when popping?** — It avoids an unnecessary copy of string/array
    values on the hot operand-stack path while preserving semantics. The optimization is documented
    in the performance section of `docs/FINAL-STATUS.md`.

38. **How does `--trace` avoid corrupting program output?** — Trace goes to a separate stderr stream;
    program output remains on stdout. See `VmOptions` in `src/runtime/vm.hpp`.

39. **How are integer overflow and division by zero handled?** — Checked helper functions detect
    overflow and the arithmetic dispatch checks zero divisors, returning `R002` or `R001`. See the
    arithmetic cases in `src/runtime/vm.cpp`.

40. **What does `build -o` produce today?** — A deterministic disassembly text artifact, not a
    serialized executable bytecode module. This is an explicit limitation in `docs/FINAL-STATUS.md`.

## Testing, trade-offs, and next steps

41. **What does CTest cover?** — Seven tests: unit tests, walking skeleton, fuzz smoke, examples,
    and three golden dumps. `CMakeLists.txt` registers them.

42. **What does the negative corpus runner assert?** — It runs each fixture with `.expected` or
    `.stderr` and `.exitcode`, then checks exit status and a diagnostic fragment. See
    `scripts/run-errors.py`.

43. **Why keep both unit and end-to-end tests?** — Unit tests isolate stage contracts; source fixtures
    prove the connected pipeline and user-visible diagnostics. The strategy is summarized in `README.md`.

44. **What does the fuzz smoke test prove?** — It gives bounded crash/hang resistance for arbitrary
    lexer/parser input; it is not a proof of semantic correctness. See `tests/fuzz/lexer_parser_fuzz.cpp`.

45. **What CI platforms are verified?** — The GitHub Actions run on October 25, 2026 passed Ubuntu,
    macOS, and Windows jobs after the MinGW warning fix. Local native macOS/Windows execution is not
    available in this environment; that distinction is recorded in `docs/FINAL-STATUS.md`.

46. **Why were warnings treated as errors?** — Warning-clean builds catch portability and correctness
    issues early. Non-MSVC flags are in `CMakeLists.txt`; MSVC uses `/W4 /WX`.

47. **What was the Windows CI failure?** — MinGW GCC 15 emitted `-Wmaybe-uninitialized` through
    `std::variant` when a temporary `Constant` was passed to `add_constant`. The fix uses typed
    overloads and an explicit integer cast in `src/codegen/code_generator.cpp`.

48. **What are the main trade-offs of the project?** — The implementation favors a small, inspectable
    compiler over generalized type/function machinery, optimizing passes, and a serialized module
    format. Those choices reduce scope but limit performance and extensibility.

49. **What would you do next for reliability?** — Add permanent tests for every newly proposed
    language extension, run native platform builds locally or in a second CI workflow, and turn the
    package/self-review checks into a required release job.

50. **What would you do next for the backend?** — Decide whether the assignment requires serialized
    bytecode; if so, define a stable module format and loader before adding optimization.

51. **What would you do next for the language?** — Add array mutation only after specifying aliasing,
    bounds, and assignment semantics; then update AST, sema, bytecode, VM, and negative tests together.

52. **What remains stretch work?** — REPL, records, closures, modules/imports, optimizer, C backend,
    and a full debugger are not implemented. See the cuts list in `docs/FINAL-STATUS.md`.

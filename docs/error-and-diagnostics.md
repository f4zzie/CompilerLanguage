# Error and diagnostics policy

## Message shape

The renderer prints:

```text
file.kite:line:column: CODE message
source line
    ^~~~
```

The primary span is underlined. Notes follow as `note: ...` and hints as `help: ...`. Diagnostics
are collected up to `--max-errors N`, sorted by file, line, column, and code, then printed to stderr.
Warnings do not change the exit code; errors do.

## Compile-time taxonomy

| Code | Message template | Trigger | Negative test |
|---|---|---|---|
| L001 | `illegal character '<char>'` | Byte is not valid in the lexical grammar. | `errors/illegal-character.src` |
| L002 | `unterminated block comment` | EOF before `*/`. | `errors/unterminated-comment.src` |
| L003 | `unterminated string literal` | EOF before closing quote. | `errors/unterminated-string.src` |
| L004 | `invalid escape sequence` | Unknown string escape. | `errors/invalid-escape.src` |
| P001 | `unexpected token <token>` | Token cannot begin the expected production. | `errors/unexpected-token.src` |
| P002 | `expected <thing>` | Required delimiter or keyword is missing. | `errors/missing-semicolon.src` |
| P003 | `invalid assignment target` | Assignment target is not a name or index. | `errors/invalid-assignment.src` |
| S001 | `duplicate declaration '<name>'` | Same-scope name redeclaration. | `errors/duplicate-name.src` |
| S002 | `unresolved name '<name>'` | Name lookup fails. | `errors/unresolved-name.src` |
| S003 | `invalid forward reference` | Local used before declaration. | `errors/local-forward-reference.src` |
| T001 | `type mismatch: expected X, found Y` | Annotation, assignment, or initializer mismatch. | `errors/type-mismatch.src` |
| T002 | `condition must have type bool` | Control-flow condition is not boolean. | `errors/non-bool-condition.src` |
| T003 | `value is not callable` | Call target is not a function/builtin. | `errors/non-callable.src` |
| T004 | `wrong argument count: expected X, found Y` | Call arity differs. | `errors/wrong-arity.src` |
| T005 | `argument N has type X, expected Y` | Call argument mismatch. | `errors/wrong-argument-type.src` |
| T006 | `return type mismatch: expected X, found Y` | Return expression differs from function type. | `errors/return-mismatch.src` |
| T007 | `operator OP does not accept X and Y` | Invalid operand types. | `errors/bad-operator.src` |
| T008 | `function may not return a value` or `missing return` | Return paths do not satisfy signature. | `errors/missing-return.src` |

## Runtime taxonomy

| Code | Message | Exit |
|---|---|---:|
| R001 | division or modulo by zero | 3 |
| R002 | integer overflow | 3 |
| R003 | index out of range | 3 |
| R004 | call stack overflow | 3 |
| R005 | invalid integer input | 3 |
| R006 | malformed bytecode module | 3 |

## Recovery strategy

The lexer consumes one invalid byte and continues. The parser inserts an error node only where the
AST contract requires a child; otherwise it synchronizes at semicolon, closing brace, EOF, or a
top-level declaration keyword. Semantic analysis substitutes `Error` types after a failed lookup or
operator check so later statements can still be checked without cascades. At the configured cap,
the collector stops adding errors and emits one final note stating that the limit was reached.

# Coding standards

## C++

- Compile as C++20 with extensions disabled.
- Use two spaces, 100-column formatting, and the repository `.clang-format` file.
- Types and classes use `PascalCase`; functions and variables use `snake_case`; constants use
  `kPascalCase` or scoped enum members.
- Prefer `std::string_view` for non-owning input and `std::unique_ptr` for AST ownership.
- Do not use raw owning pointers, exceptions for normal compile errors, global mutable state, or
  template metaprogramming.
- Keep public contracts in headers and implementation details in `.cpp` files.
- Use `[[nodiscard]]` for query functions whose result matters.
- Every `TODO` must include a task-card ID, for example `TODO(T012)`.

## Comments and documentation

Comment invariants and decisions, not syntax. Update the language spec and architecture document
when changing a public grammar, AST, bytecode, or diagnostic contract. Examples must have exact
expected output.

## Commits and branches

Use `feat:`, `fix:`, `test:`, `docs:`, `refactor:`, or `chore:` followed by a concise imperative
description. Branches use `feat/T###-name`, `fix/T###-name`, or `docs/T###-name`. Keep branches
short-lived and merge through a pull request.

## Review definition of done

The task-card acceptance criteria pass, relevant unit and end-to-end tests are present, Debug builds
with warnings as errors, CTest was run, no generated output is committed, and affected docs are
updated.

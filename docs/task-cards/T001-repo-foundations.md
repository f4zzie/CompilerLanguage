ID:              T001
Title:           Repository foundations
Milestone:       M0
Tier:            MUST
Depends on:      []
Blocks:          [T002, T003, T004]
Owner:           (unassigned)
Estimate:        2 / 4 / 6 hours
Files to create or modify:   CMakeLists.txt; CMakePresets.json; .gitignore; .clang-format; CONTRIBUTING.md; .github/workflows/ci.yml; scripts/*
Description:     Initialize Git, add the CMake targets and presets, configure warnings as errors, add Debug/Release/sanitizer options, and create cross-platform helper-script entry points.
Inputs / Outputs: exact data contracts, referencing architecture.md: Input is an empty repository. Output is a configureable CMake project with `kite` and `kite_tests` targets and CTest registration.
Acceptance criteria:   1. `cmake --preset linux-debug` configures. 2. `cmake --build --preset linux-debug` builds. 3. CTest discovers `kite_unit_tests` and `walking_skeleton`. 4. CI YAML contains Linux/macOS/Windows jobs.
Tests to write first: names and what they assert: `tests/test_main.cpp`; asserts the test binary starts and registers tests.
Edge cases to cover: explicit C++20 mode; warnings treated as errors; sanitizer option is off by default; scripts do not assume a Unix-only path when run on Windows.
Out of scope:          compiler semantics, package publishing, deployment, and generated build output.
Definition of done:    code merged, tests green, docs updated, dump flag works

# Assumptions and change points

Review date: October 25, 2026. The assignment rubric and platform execution environment were not
supplied, so the following assumptions remain explicit rather than being presented as verified facts.

| Assumption | Default used | How to change it |
|---|---|---|
| Native code requirement | No; bytecode plus VM is acceptable | If the assignment says native code, promote C-transpile to MUST in ADR-0002, add `src/target_c`, and replace the release schedule for VM-only extras. |
| Deadline | Saturday, December 5, 2026 | Change the single `DEADLINE` value in `docs/milestones.md`, then recompute every date and preserve the final 15–20% and 10% buffer. |
| Rubric | Not available; generic compiler-course criteria are used | Replace the generic rows in `docs/rubric-traceability.md` one-for-one with the supplied rubric. |
| Implementation language | C++20 | If prohibited, create ADR-0001 revision and translate the public contracts to the mandated language. |
| Test framework | Upstream doctest v2.5.3 single header is vendored at `third_party/doctest/doctest.h` | Upgrade only by selecting an official upstream release, replacing the header, preserving its license text, and recording the source URL/version in `third_party/doctest/README.md`. |
| Platforms | Linux, macOS, Windows/MSVC; MinGW is best effort | If Linux-only is selected, remove the Windows/macOS CI matrix and state the reduced portability guarantee. |
| Team | Three students, 8 hours per person per week | For another team size, use the parallel plans in `docs/milestones.md`; MUST scope remains intentionally one-person achievable. |
| Entry point | Top-level statements compile into synthetic `__main` | If a course requires `main`, add a source-level `main() -> unit` rule and update grammar, sema, codegen, and examples. |
| Output | `print` writes a value plus newline to stdout | Change the builtin contract and every `.expected` file together. |
| Source encoding | UTF-8 bytes; ASCII identifiers | Add Unicode identifier decoding only as a documented stretch feature. |
| Error cap | 50 errors by default, configurable with `--max-errors` | Change `PipelineOptions` default and CLI parsing; preserve deterministic ordering. |

## Open questions and exceptions

1. Confirm that the assignment accepts a custom bytecode VM; local implementation assumes yes.
2. Replace the placeholder deadline if the course provides a different date; the local plan uses
   December 5, 2026.
3. Replace the generic rubric rows when the official rubric is supplied; no official rubric was
   available during this review.
4. Confirm whether the instructor requires a source-level `main` function; local implementation uses
   synthetic top-level `__main`.
5. Run the macOS, Windows/MSVC, PowerShell, and GitHub Actions paths in their native environments;
   only Linux execution was available during this review.

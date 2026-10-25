#!/usr/bin/env python3
from __future__ import annotations

import sys
from pathlib import Path


COMMANDS = (
    "cmake --preset linux-debug",
    "cmake --build --preset linux-debug",
    "ctest --test-dir build/linux-debug --output-on-failure",
    "./scripts/test-all",
    "cmake --preset linux-asan",
    "cmake --build --preset linux-asan",
    "ctest --test-dir build/linux-asan --output-on-failure",
    "cmake --preset release",
    "cmake --build --preset release",
    "ctest --test-dir build/release --output-on-failure",
    "python3 scripts/check-examples.py build/linux-debug/kite examples",
    "python3 scripts/run-corpus.py build/linux-debug/kite tests/programs",
    "python3 scripts/run-errors.py build/linux-debug/kite tests/errors",
    "./scripts/package-submission.sh",
)


def check_rubric(path: Path) -> list[str]:
    problems: list[str] = []
    rows = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.startswith("|") or line.startswith("|---"):
            continue
        fields = [field.strip() for field in line.replace("|>", "PIPEOP").strip("|").split("|")]
        if len(fields) == 4 and fields[0] != "Generic criterion":
            rows.append(fields)
    if not rows:
        return ["rubric-traceability.md has no data rows"]
    for index, row in enumerate(rows, start=1):
        if any(not field for field in row):
            problems.append(f"rubric row {index} has an empty field")
        if not all("`" in field for field in row[1:3]):
            problems.append(f"rubric row {index} lacks a concrete file/command reference")
    return problems


def main() -> int:
    root = Path(__file__).resolve().parent.parent
    required = [
        root / "docs/FINAL-STATUS.md",
        root / "docs/CODE-WALKTHROUGH.md",
        root / "docs/rubric-traceability.md",
        root / "report/evidence.md",
    ]
    missing = [str(path.relative_to(root)) for path in required if not path.exists()]
    cards = sorted((root / "docs/task-cards").glob("T*.md"))
    ids = [path.stem.split("-", 1)[0] for path in cards]
    expected = [f"T{index:03d}" for index in range(1, 30)]
    status = (root / "docs/FINAL-STATUS.md").read_text(encoding="utf-8")
    progress = (root / "docs/PROGRESS.md").read_text(encoding="utf-8")
    missing_commands = [command for command in COMMANDS if command not in status]
    rubric_problems = check_rubric(root / "docs/rubric-traceability.md") if not missing else ["rubric file missing"]
    problems = missing + rubric_problems
    if ids != expected:
        problems.append(f"task IDs are {ids}")
    if missing_commands:
        problems.append(f"missing verified commands: {missing_commands}")
    if "| T029 | done |" not in status or "| T029 | done |" not in progress:
        problems.append("T029 is not marked done in final status and progress")
    digest = root / "submission.zip.sha256"
    if not digest.exists() or len(digest.read_text(encoding="utf-8").split()) < 1:
        problems.append("submission.zip.sha256 is missing")
    if problems:
        print("; ".join(problems), file=sys.stderr)
        return 1
    print(f"self-review paths/rubric/commands ok: {len(cards)} task cards")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

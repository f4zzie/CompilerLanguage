#!/usr/bin/env python3
"""Run Kite positive and negative corpus fixtures with deterministic comparisons."""

from __future__ import annotations

import difflib
import subprocess
import sys
from pathlib import Path


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8") if path.exists() else ""


def run_case(binary: Path, source: Path) -> tuple[bool, str]:
    expected = read_text(source.with_suffix(".expected"))
    exit_path = source.with_suffix(".exitcode")
    expected_exit = int(read_text(exit_path).strip() or "0")
    stdin = read_text(source.with_suffix(".stdin"))
    expected_stderr = read_text(source.with_suffix(".stderr"))
    try:
        result = subprocess.run(
            [str(binary), "run", str(source)],
            input=stdin,
            text=True,
            capture_output=True,
            timeout=2,
            check=False,
        )
    except subprocess.TimeoutExpired:
        return False, f"{source}: timed out after 2 seconds"
    if result.returncode != expected_exit:
        return False, f"{source}: exit {result.returncode}, expected {expected_exit}"
    if result.stdout != expected:
        diff = "".join(difflib.unified_diff(expected.splitlines(True), result.stdout.splitlines(True), fromfile="expected", tofile="actual"))
        return False, f"{source}: stdout mismatch\n{diff}"
    if expected_stderr and expected_stderr not in result.stderr:
        return False, f"{source}: stderr did not contain expected fragment {expected_stderr!r}"
    return True, f"PASS {source}"


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: run-corpus.py KITE_BINARY CORPUS_DIR", file=sys.stderr)
        return 2
    binary = Path(sys.argv[1]).resolve()
    corpus = Path(sys.argv[2])
    sources = sorted((*corpus.glob("*.src"), *corpus.glob("*.kite")))
    failures = 0
    for source in sources:
        passed, message = run_case(binary, source)
        print(message)
        failures += not passed
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())

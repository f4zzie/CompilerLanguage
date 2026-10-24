#!/usr/bin/env python3
"""Run Kite negative fixtures and check expected exit/code/message fragments."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: run-errors.py KITE_BINARY ERROR_DIR", file=sys.stderr)
        return 2
    binary = Path(sys.argv[1]).resolve()
    error_dir = Path(sys.argv[2])
    failures = 0
    for source in sorted((*error_dir.glob("*.src"), *error_dir.glob("*.kite"))):
        expected_path = source.with_suffix(".expected")
        exit_path = source.with_suffix(".exitcode")
        if not expected_path.exists() or not exit_path.exists():
            continue
        expected_exit = int(exit_path.read_text(encoding="utf-8").strip())
        result = subprocess.run([str(binary), "run", str(source)], text=True, capture_output=True, timeout=2, check=False)
        stderr_path = source.with_suffix(".stderr")
        expected = (stderr_path if stderr_path.exists() else expected_path).read_text(encoding="utf-8").strip()
        actual = result.stderr
        if result.returncode != expected_exit or expected not in actual:
            failures += 1
            print(f"FAIL {source}: exit={result.returncode} expected={expected_exit}; stderr={actual!r}")
        else:
            print(f"PASS {source}")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())

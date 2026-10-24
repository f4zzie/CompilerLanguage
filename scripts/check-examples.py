#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path


def main() -> int:
    binary = Path(sys.argv[1]).resolve()
    examples = Path(sys.argv[2])
    failures = 0
    for source in sorted(examples.glob("*.kite")):
        expected_path = source.with_suffix(".expected")
        if not expected_path.exists():
            print(f"FAIL {source}: missing expected output")
            failures += 1
            continue
        result = subprocess.run([str(binary), "run", str(source)], text=True, capture_output=True, timeout=2, check=False)
        expected = expected_path.read_text(encoding="utf-8")
        if result.returncode != 0 or result.stdout != expected:
            print(f"FAIL {source}: exit={result.returncode}, stdout={result.stdout!r}, expected={expected!r}")
            failures += 1
        else:
            print(f"PASS {source}")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())

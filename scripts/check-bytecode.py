#!/usr/bin/env python3
"""Round-trip every example through build/exec and exercise malformed files."""

from __future__ import annotations

import subprocess
import sys
import tempfile
from pathlib import Path


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, text=True, capture_output=True, timeout=5, check=False)


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: check-bytecode.py KITE_BINARY EXAMPLES", file=sys.stderr)
        return 2
    binary = str(Path(sys.argv[1]).resolve())
    examples = Path(sys.argv[2])
    failures = 0
    with tempfile.TemporaryDirectory(prefix="kite-bytecode-") as directory:
        temporary = Path(directory)
        for source in sorted(examples.glob("*.kite")):
            expected = source.with_suffix(".expected").read_text(encoding="utf-8")
            source_run = run([binary, "run", str(source)])
            bytecode = temporary / (source.stem + ".kbc")
            build = run([binary, "build", str(source), "-o", str(bytecode)])
            execute = run([binary, "exec", str(bytecode)]) if build.returncode == 0 else None
            if (
                source_run.returncode != 0
                or source_run.stdout != expected
                or build.returncode != 0
                or execute is None
                or execute.returncode != 0
                or execute.stdout != source_run.stdout
            ):
                failures += 1
                print(f"FAIL {source}: run={source_run.returncode}, build={build.returncode}, "
                      f"exec={None if execute is None else execute.returncode}")
            else:
                print(f"PASS {source}")

        truncated = temporary / "truncated.kbc"
        truncated.write_bytes(b"KBC\0\1")
        corrupt = run([binary, "exec", str(truncated)])
        if corrupt.returncode != 3 or "R006" not in corrupt.stderr:
            failures += 1
            print(f"FAIL truncated bytecode: exit={corrupt.returncode}, stderr={corrupt.stderr!r}")
        else:
            print("PASS truncated bytecode")

        invalid = temporary / "invalid.kbc"
        invalid.write_bytes(b"nope")
        corrupt = run([binary, "exec", str(invalid)])
        if corrupt.returncode != 3 or "R006" not in corrupt.stderr:
            failures += 1
            print(f"FAIL corrupt bytecode: exit={corrupt.returncode}, stderr={corrupt.stderr!r}")
        else:
            print("PASS corrupt bytecode")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())

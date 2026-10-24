#!/usr/bin/env python3
from __future__ import annotations

import sys
import zipfile

REQUIRED = {
    "README.md",
    "PLAN.md",
    "CMakeLists.txt",
    "CMakePresets.json",
    "docs/language-spec.md",
    "docs/architecture.md",
    "src/driver/main.cpp",
}


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: check-package.py ARCHIVE", file=sys.stderr)
        return 2
    with zipfile.ZipFile(sys.argv[1]) as archive:
        names = {name.rstrip("/") for name in archive.namelist()}
    missing = sorted(REQUIRED - names)
    forbidden = sorted(
        name for name in names if name.startswith("build/") or name.startswith(".git/") or name.endswith(".log")
    )
    if missing or forbidden:
        print(f"missing={missing} forbidden={forbidden}", file=sys.stderr)
        return 1
    print(f"package manifest ok: {len(names)} entries")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

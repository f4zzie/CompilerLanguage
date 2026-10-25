#!/usr/bin/env python3
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main() -> int:
    unit_cases = unit_assertions = 0
    for path in sorted((*((ROOT / "tests/unit").glob("*.cpp")), *((ROOT / "tests").glob("*.cpp")))):
        text = path.read_text(encoding="utf-8")
        unit_cases += len(re.findall(r"\bTEST_CASE\s*\(", text))
        unit_assertions += len(re.findall(r"\b(?:CHECK|REQUIRE)(?:_FALSE|_MESSAGE|_THROWS|_EQ|_NE)?\s*\(", text))
    golden = 3
    e2e = len(list((ROOT / "examples").glob("*.kite"))) + len(list((ROOT / "tests/programs").glob("*.src"))) + 1
    error_sources = [*((ROOT / "tests/errors").glob("*.src")), *((ROOT / "tests/errors").glob("*.kite"))]
    negative = sum(
        1
        for path in error_sources
        if path.with_suffix(".expected").exists() and path.with_suffix(".exitcode").exists()
    )
    fuzz = 1000
    print("kind,cases,assertions")
    print(f"unit,{unit_cases},{unit_assertions}")
    print(f"golden,{golden},{golden}")
    print(f"end-to-end,{e2e},{e2e * 2}")
    print(f"negative,{negative},{negative * 2}")
    print(f"fuzz,1,{fuzz}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/.." && pwd)"
cd "$repo_root"

cmake --preset linux-debug
cmake --build --preset linux-debug
for command in tokens ast disasm; do
  build/linux-debug/kite "$command" tests/golden/hello.kite > "tests/golden/hello.$command"
done
echo "Updated golden fixtures; review the diff before committing."

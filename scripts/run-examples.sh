#!/usr/bin/env bash
set -euo pipefail

if [[ ! -x build/linux-debug/kite ]]; then
  cmake --preset linux-debug
  cmake --build --preset linux-debug
fi

for file in examples/*.kite; do
  echo "== $file =="
  build/linux-debug/kite run "$file"
done

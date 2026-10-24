$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot
try {
  # CMake's KITE_RUN_WALKING_SKELETON switch stays OFF until T014.
  cmake --preset linux-debug
  cmake --build --preset linux-debug
  ctest --test-dir build/linux-debug --output-on-failure
}
finally {
  Pop-Location
}

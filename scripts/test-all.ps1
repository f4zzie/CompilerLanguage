$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot
try {
  cmake --preset linux-debug
  cmake --build --preset linux-debug
  ctest --test-dir build/linux-debug --output-on-failure
}
finally {
  Pop-Location
}

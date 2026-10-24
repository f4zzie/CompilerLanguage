$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot
try {
  cmake --preset linux-debug
  cmake --build --preset linux-debug
}
finally {
  Pop-Location
}

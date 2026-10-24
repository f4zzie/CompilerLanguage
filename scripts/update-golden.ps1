$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot
try {
  cmake --preset linux-debug
  cmake --build --preset linux-debug
  foreach ($command in @("tokens", "ast", "disasm")) {
    & build/linux-debug/kite $command tests/golden/hello.kite | Out-File -Encoding utf8 "tests/golden/hello.$command"
  }
  Write-Host "Updated golden fixtures; review the diff before committing."
}
finally {
  Pop-Location
}

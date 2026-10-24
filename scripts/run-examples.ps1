$ErrorActionPreference = "Stop"
if (-not (Test-Path build/linux-debug/kite.exe)) {
  cmake --preset linux-debug
  cmake --build --preset linux-debug
}
Get-ChildItem examples -Filter *.kite | ForEach-Object {
  Write-Host "== $($_.FullName) =="
  & build/linux-debug/kite.exe run $_.FullName
}

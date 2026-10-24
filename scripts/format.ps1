$ErrorActionPreference = "Stop"
Get-ChildItem -Path src,tests -Recurse -Include *.cpp,*.hpp | ForEach-Object {
  clang-format -i $_.FullName
}

@echo off
setlocal
set "REPO_ROOT=%~dp0.."
pushd "%REPO_ROOT%"
if errorlevel 1 exit /b 1

cmake --preset linux-debug
if errorlevel 1 (
  popd
  exit /b 1
)
cmake --build --preset linux-debug
if errorlevel 1 (
  popd
  exit /b 1
)
ctest --test-dir build/linux-debug --output-on-failure
set "EXIT_CODE=%ERRORLEVEL%"
popd
exit /b %EXIT_CODE%

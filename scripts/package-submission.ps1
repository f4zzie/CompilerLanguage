$ErrorActionPreference = "Stop"
if (Test-Path submission.zip) { Remove-Item submission.zip }
Compress-Archive -Path * -DestinationPath submission.zip -Force
python3 scripts/check-package.py submission.zip
Write-Host "Created submission.zip; inspect it before submitting."

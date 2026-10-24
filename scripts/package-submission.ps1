$ErrorActionPreference = "Stop"
if (Test-Path submission.zip) { Remove-Item submission.zip }
Compress-Archive -Path * -DestinationPath submission.zip -Force
Write-Host "Created submission.zip; inspect it before submitting."

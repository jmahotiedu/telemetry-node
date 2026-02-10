# Run protocol unit test using venv. Create venv first with: .\setup_venv.ps1
$venvPy = Join-Path $PSScriptRoot ".venv\Scripts\python.exe"
if (-not (Test-Path $venvPy)) {
    Write-Error "Run .\setup_venv.ps1 first to create the venv."
    exit 1
}
& $venvPy (Join-Path $PSScriptRoot "protocol.py")
exit $LASTEXITCODE

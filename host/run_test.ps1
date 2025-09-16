# Run protocol unit test using venv. Create venv first with: .\setup_venv.ps1
$venvPy = Join-Path $PSScriptRoot ".venv\Scripts\python.exe"
if (-not (Test-Path $venvPy)) {
    Write-Error "Run .\setup_venv.ps1 first to create the venv."
    exit 1
}
& $venvPy -m unittest discover -s (Join-Path $PSScriptRoot "tests") -p "test_*.py"
exit $LASTEXITCODE

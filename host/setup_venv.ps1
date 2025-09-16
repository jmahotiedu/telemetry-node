# Create venv and install dependencies. Run with: .\setup_venv.ps1
# Requires: python or py on PATH (e.g. from python.org or Windows Store)
$py = $null
foreach ($cmd in @('python', 'py', 'python3')) {
    try {
        $null = Get-Command $cmd -ErrorAction Stop
        $py = $cmd
        break
    } catch {}
}
if (-not $py) {
    Write-Error "Python not found. Install from python.org or add python to PATH."
    exit 1
}

$venv = Join-Path $PSScriptRoot ".venv"
if (-not (Test-Path $venv)) {
    & $py -m venv $venv
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
& (Join-Path $venv "Scripts\pip.exe") install --quiet -r (Join-Path $PSScriptRoot "requirements.txt")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "venv ready at $venv. Activate with: .\.venv\Scripts\Activate.ps1"
Write-Host "Run test: .\.venv\Scripts\python.exe -m unittest discover -s tests -p test_*.py"

# PowerShell Build Script for Library Management System

param(
    [Parameter(Position=0)]
    [ValidateSet("all", "clean", "cleanall", "rebuild", "run", "test", "help")]
    [string]$Target = "all"
)

$MAKE = "C:\msys64\ucrt64\bin\mingw32-make.exe"

# Check if make exists
if (-not (Test-Path $MAKE)) {
    Write-Host "Error: mingw32-make not found at $MAKE" -ForegroundColor Red
    Write-Host "Please install MSYS2 or update the path in this script." -ForegroundColor Yellow
    exit 1
}

# Run make with the specified target
Write-Host "Running: $MAKE $Target" -ForegroundColor Cyan
& $MAKE $Target

# Check exit code
if ($LASTEXITCODE -eq 0) {
    Write-Host "`nBuild successful!" -ForegroundColor Green
} else {
    Write-Host "`nBuild failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

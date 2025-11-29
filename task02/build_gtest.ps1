# Build script for Rock-Scissors-Paper-Lizard-Spock game
# This script builds the project and runs tests using CMake

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Change to build directory
Set-Location build

Write-Host "Configuring CMake..." -ForegroundColor Cyan
cmake -G "MinGW Makefiles" ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "`nBuilding project..." -ForegroundColor Cyan
cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "`nRunning tests..." -ForegroundColor Cyan
ctest --output-on-failure

if ($LASTEXITCODE -ne 0) {
    Write-Host "`nTests failed!" -ForegroundColor Red
} else {
    Write-Host "`nAll tests passed!" -ForegroundColor Green
}

Set-Location ..

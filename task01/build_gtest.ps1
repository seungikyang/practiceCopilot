# PowerShell script to build and run Google Test

# Colors for output
function Write-Success { Write-Host $args -ForegroundColor Green }
function Write-Error { Write-Host $args -ForegroundColor Red }
function Write-Info { Write-Host $args -ForegroundColor Cyan }

Write-Info "=========================================="
Write-Info "Building URL Tools with Google Test"
Write-Info "=========================================="

# Create build directory
$BUILD_DIR = "build"
if (Test-Path $BUILD_DIR) {
    Write-Info "Cleaning existing build directory..."
    Remove-Item -Recurse -Force $BUILD_DIR
}

Write-Info "Creating build directory..."
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

# Navigate to build directory
Set-Location $BUILD_DIR

# Run CMake configuration
Write-Info "`nConfiguring with CMake..."
cmake .. -G "MinGW Makefiles"

if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed!"
    Set-Location ..
    exit 1
}

Write-Success "CMake configuration successful!"

# Build the project
Write-Info "`nBuilding project..."
cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed!"
    Set-Location ..
    exit 1
}

Write-Success "Build successful!"

# Run tests
Write-Info "`n=========================================="
Write-Info "Running Google Tests"
Write-Info "=========================================="

ctest --output-on-failure

if ($LASTEXITCODE -eq 0) {
    Write-Success "`n=========================================="
    Write-Success "All tests passed!"
    Write-Success "=========================================="
} else {
    Write-Error "`n=========================================="
    Write-Error "Some tests failed!"
    Write-Error "=========================================="
}

# Return to original directory
Set-Location ..

exit $LASTEXITCODE

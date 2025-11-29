# PowerShell build script for URL Tools tests

# Colors for output
function Write-Success { Write-Host $args -ForegroundColor Green }
function Write-Error { Write-Host $args -ForegroundColor Red }
function Write-Info { Write-Host $args -ForegroundColor Cyan }

# Configuration
$CC = "gcc"
$CFLAGS = "-Wall", "-Wextra", "-g", "-std=c99"
$SRC_DIR = "src"
$TEST_DIR = "tests"
$BUILD_DIR = "build"

# Source files
$SOURCES = @(
    "$SRC_DIR/url_tools.c"
)
$TEST_SOURCES = @(
    "$TEST_DIR/test_url_tools.c"
)

# Output executable
$TEST_EXEC = "$BUILD_DIR/test_url_tools.exe"

# Create build directory
Write-Info "Creating build directory..."
if (-not (Test-Path $BUILD_DIR)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}

# Compile tests
Write-Info "Compiling tests..."
$compileArgs = @($CFLAGS) + @("-o", $TEST_EXEC) + $SOURCES + $TEST_SOURCES
Write-Info "Command: $CC $compileArgs"

& $CC @compileArgs

if ($LASTEXITCODE -eq 0) {
    Write-Success "Compilation successful!"
    
    # Run tests
    Write-Info "`nRunning tests..."
    & ".\$TEST_EXEC"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "`nAll tests passed!"
        exit 0
    } else {
        Write-Error "`nSome tests failed!"
        exit 1
    }
} else {
    Write-Error "Compilation failed!"
    exit 1
}

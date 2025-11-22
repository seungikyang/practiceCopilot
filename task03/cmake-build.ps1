# CMake Build Script for Library Management System

param(
    [Parameter(Position=0)]
    [ValidateSet("configure", "build", "test", "clean", "rebuild", "install", "help")]
    [string]$Action = "build",
    
    [Parameter()]
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$BuildType = "Debug",
    
    [Parameter()]
    [string]$Generator = "MinGW Makefiles"
)

$BUILD_DIR = "build"
$CMAKE = "cmake"

function Show-Help {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  CMake Build Script for Library System" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage: .\cmake-build.ps1 [Action] [-BuildType <type>] [-Generator <gen>]"
    Write-Host ""
    Write-Host "Actions:" -ForegroundColor Yellow
    Write-Host "  configure  - Configure CMake build"
    Write-Host "  build      - Build the project (default)"
    Write-Host "  test       - Build and run tests"
    Write-Host "  clean      - Clean build directory"
    Write-Host "  rebuild    - Clean and rebuild"
    Write-Host "  install    - Install the project"
    Write-Host "  help       - Show this help message"
    Write-Host ""
    Write-Host "Options:" -ForegroundColor Yellow
    Write-Host "  -BuildType   Debug (default), Release, RelWithDebInfo, MinSizeRel"
    Write-Host "  -Generator   'MinGW Makefiles' (default), 'Ninja', 'Visual Studio 17 2022'"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Green
    Write-Host "  .\cmake-build.ps1 configure"
    Write-Host "  .\cmake-build.ps1 build -BuildType Release"
    Write-Host "  .\cmake-build.ps1 test"
    Write-Host "  .\cmake-build.ps1 rebuild -BuildType Release"
    Write-Host ""
}

function Test-CMake {
    try {
        $null = & $CMAKE --version 2>&1
        return $true
    } catch {
        return $false
    }
}

function Configure-Project {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  Configuring CMake Project" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Build Type: $BuildType" -ForegroundColor Yellow
    Write-Host "Generator:  $Generator" -ForegroundColor Yellow
    Write-Host ""
    
    if (-not (Test-Path $BUILD_DIR)) {
        New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    }
    
    Push-Location $BUILD_DIR
    try {
        & $CMAKE -G $Generator -DCMAKE_BUILD_TYPE=$BuildType ..
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed"
        }
        Write-Host ""
        Write-Host "Configuration completed successfully!" -ForegroundColor Green
        Write-Host ""
    } finally {
        Pop-Location
    }
}

function Build-Project {
    if (-not (Test-Path $BUILD_DIR)) {
        Write-Host "Build directory not found. Configuring first..." -ForegroundColor Yellow
        Configure-Project
    }
    
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  Building Project" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    
    Push-Location $BUILD_DIR
    try {
        & $CMAKE --build . --config $BuildType
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed"
        }
        Write-Host ""
        Write-Host "Build completed successfully!" -ForegroundColor Green
        Write-Host "Executable: build\bin\library.exe" -ForegroundColor Green
        Write-Host ""
    } finally {
        Pop-Location
    }
}

function Test-Project {
    Build-Project
    
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  Running Tests" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    
    Push-Location $BUILD_DIR
    try {
        & $CMAKE --build . --target check --config $BuildType
        if ($LASTEXITCODE -ne 0) {
            Write-Host ""
            Write-Host "Some tests failed!" -ForegroundColor Red
            Write-Host ""
            exit 1
        }
        Write-Host ""
        Write-Host "All tests passed!" -ForegroundColor Green
        Write-Host ""
    } finally {
        Pop-Location
    }
}

function Clean-Project {
    Write-Host ""
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR
        Write-Host "Build directory cleaned!" -ForegroundColor Green
    } else {
        Write-Host "Build directory does not exist." -ForegroundColor Yellow
    }
    Write-Host ""
}

function Rebuild-Project {
    Clean-Project
    Configure-Project
    Build-Project
}

function Install-Project {
    Build-Project
    
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  Installing Project" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    
    Push-Location $BUILD_DIR
    try {
        & $CMAKE --install . --config $BuildType
        if ($LASTEXITCODE -ne 0) {
            throw "Installation failed"
        }
        Write-Host ""
        Write-Host "Installation completed!" -ForegroundColor Green
        Write-Host ""
    } finally {
        Pop-Location
    }
}

# Main script execution
try {
    # Check if CMake is installed
    if (-not (Test-CMake)) {
        Write-Host ""
        Write-Host "Error: CMake is not installed or not in PATH" -ForegroundColor Red
        Write-Host ""
        Write-Host "Please install CMake:" -ForegroundColor Yellow
        Write-Host "  - Download from: https://cmake.org/download/"
        Write-Host "  - Or use: winget install Kitware.CMake"
        Write-Host "  - Or use: choco install cmake"
        Write-Host ""
        exit 1
    }
    
    switch ($Action) {
        "configure" { Configure-Project }
        "build"     { Build-Project }
        "test"      { Test-Project }
        "clean"     { Clean-Project }
        "rebuild"   { Rebuild-Project }
        "install"   { Install-Project }
        "help"      { Show-Help }
        default     { Show-Help }
    }
    
    exit 0
    
} catch {
    Write-Host ""
    Write-Host "Error: $_" -ForegroundColor Red
    Write-Host ""
    exit 1
}

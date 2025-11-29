param(
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$src = Join-Path $PSScriptRoot 'src'
$bin = Join-Path $PSScriptRoot 'bin'

if ($Clean) {
    if (Test-Path $bin) { Remove-Item $bin -Recurse -Force }
}

if (!(Test-Path $bin)) { New-Item -ItemType Directory -Path $bin | Out-Null }

$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    Write-Error 'gcc not found. Please install MinGW or set PATH.'
}

Push-Location $src
try {
    $cmd = @(
        'gcc',
        '-std=c99','-Wall','-Wextra','-O2',
        'main.c','game.c',
        '-o', (Join-Path $bin 'rock_scissors_paper.exe')
    )
    & $cmd[0] $cmd[1..($cmd.Length-1)]
    if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE" }
    Write-Host "Built: $(Join-Path $bin 'rock_scissors_paper.exe')"
}
finally {
    Pop-Location
}

# Build and run the tower defense map demo on Windows.
# Usage (from the project root):
#   .\run.ps1
# Optional: .\run.ps1 -Config Debug

param(
    [ValidateSet("Release", "Debug")]
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

function Test-Command($Name) {
    return [bool](Get-Command $Name -ErrorAction SilentlyContinue)
}

if (-not (Test-Command cmake)) {
    Write-Host @"
CMake was not found on your PATH.

Install one of these, then reopen the terminal:
  1) Visual Studio 2022 (Desktop development with C++) — includes CMake
  2) Or: winget install Kitware.CMake
     plus a compiler (Visual Studio Build Tools or MinGW)
"@ -ForegroundColor Red
    exit 1
}

$buildDir = "build"
$cmakeArgs = @("-S", ".", "-B", $buildDir, "-DCMAKE_BUILD_TYPE=$Config")

# Prefer an explicit generator so a bare "cmake" doesn't guess wrong.
if (Test-Command cl) {
    # Visual Studio / MSVC — multi-config; --config still used at build time.
} elseif (Test-Command g++) {
    $cmakeArgs += @("-G", "MinGW Makefiles", "-DCMAKE_CXX_COMPILER=g++")
} else {
    Write-Host @"
No C++ compiler found on PATH (cl.exe or g++.exe).

Install Visual Studio with "Desktop development with C++",
or MinGW (e.g. MSYS2), then reopen the terminal.
"@ -ForegroundColor Red
    exit 1
}

Write-Host "==> Configuring ($Config)..." -ForegroundColor Cyan
& cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "==> Building..." -ForegroundColor Cyan
cmake --build $buildDir --config $Config
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$candidates = @(
    Join-Path $buildDir "bin\$Config\towerdefense.exe"
    Join-Path $buildDir "bin\towerdefense.exe"
    Join-Path $buildDir "$Config\towerdefense.exe"
    Join-Path $buildDir "towerdefense.exe"
)

$exe = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $exe) {
    Write-Host "Build succeeded but towerdefense.exe was not found under '$buildDir'." -ForegroundColor Red
    exit 1
}

Write-Host "==> Running $exe" -ForegroundColor Green
Write-Host "    Use the on-screen menus (shop, click to place/upgrade)."
& $exe

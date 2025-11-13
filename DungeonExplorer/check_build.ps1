# Build Verification Script for Windows PowerShell
# Run this to check if you have everything needed to build the project

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Dungeon Explorer Build Checker" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

$allGood = $true

# Check CMake
Write-Host "Checking CMake..." -NoNewline
try {
    $cmakeVersion = cmake --version 2>$null
    if ($cmakeVersion) {
        Write-Host " ✓ Found" -ForegroundColor Green
        Write-Host "  $($cmakeVersion[0])" -ForegroundColor Gray
    } else {
        Write-Host " ✗ Not found" -ForegroundColor Red
        Write-Host "  Install from: https://cmake.org/download/" -ForegroundColor Yellow
        $allGood = $false
    }
} catch {
    Write-Host " ✗ Not found" -ForegroundColor Red
    $allGood = $false
}

# Check C++ Compiler
Write-Host "`nChecking C++ Compiler..." -NoNewline
$compilerFound = $false

# Check for Visual Studio
try {
    $vsPath = & "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath 2>$null
    if ($vsPath) {
        Write-Host " ✓ Visual Studio Found" -ForegroundColor Green
        Write-Host "  Path: $vsPath" -ForegroundColor Gray
        $compilerFound = $true
    }
} catch {}

# Check for MinGW
if (-not $compilerFound) {
    try {
        $gccVersion = gcc --version 2>$null
        if ($gccVersion) {
            Write-Host " ✓ GCC Found" -ForegroundColor Green
            Write-Host "  $($gccVersion[0])" -ForegroundColor Gray
            $compilerFound = $true
        }
    } catch {}
}

if (-not $compilerFound) {
    Write-Host " ✗ Not found" -ForegroundColor Red
    Write-Host "  Install Visual Studio 2019+ or MinGW-w64" -ForegroundColor Yellow
    $allGood = $false
}

# Check for vcpkg (optional but recommended)
Write-Host "`nChecking vcpkg (optional)..." -NoNewline
if (Test-Path "C:\vcpkg\vcpkg.exe") {
    Write-Host " ✓ Found" -ForegroundColor Green
    Write-Host "  Path: C:\vcpkg\" -ForegroundColor Gray
} elseif (Test-Path "$env:USERPROFILE\vcpkg\vcpkg.exe") {
    Write-Host " ✓ Found" -ForegroundColor Green
    Write-Host "  Path: $env:USERPROFILE\vcpkg\" -ForegroundColor Gray
} else {
    Write-Host " ✗ Not found (optional)" -ForegroundColor Yellow
    Write-Host "  Recommended for easy SFML/TGUI installation" -ForegroundColor Gray
}

# Check for SFML
Write-Host "`nChecking SFML..." -NoNewline
$sflmFound = $false

if ($env:SFML_DIR) {
    Write-Host " ✓ Found via SFML_DIR" -ForegroundColor Green
    Write-Host "  Path: $env:SFML_DIR" -ForegroundColor Gray
    $sflmFound = $true
} elseif (Test-Path "C:\SFML-2.6.0") {
    Write-Host " ✓ Found at C:\SFML-2.6.0" -ForegroundColor Green
    $sflmFound = $true
} else {
    Write-Host " ✗ Not found" -ForegroundColor Red
    Write-Host "  Install via vcpkg or download from https://www.sfml-dev.org/" -ForegroundColor Yellow
    $allGood = $false
}

# Check for TGUI
Write-Host "`nChecking TGUI..." -NoNewline
$tguiFound = $false

if ($env:TGUI_DIR) {
    Write-Host " ✓ Found via TGUI_DIR" -ForegroundColor Green
    Write-Host "  Path: $env:TGUI_DIR" -ForegroundColor Gray
    $tguiFound = $true
} elseif (Test-Path "C:\TGUI-1.0") {
    Write-Host " ✓ Found at C:\TGUI-1.0" -ForegroundColor Green
    $tguiFound = $true
} else {
    Write-Host " ✗ Not found" -ForegroundColor Red
    Write-Host "  Install via vcpkg or download from https://tgui.eu/" -ForegroundColor Yellow
    $allGood = $false
}

# Check project files
Write-Host "`nChecking Project Files..." -NoNewline
$requiredFiles = @(
    "CMakeLists.txt",
    "src\main.cpp",
    "include\Game.h",
    "include\DataStructures\Stack.h"
)

$filesOk = $true
foreach ($file in $requiredFiles) {
    if (-not (Test-Path $file)) {
        $filesOk = $false
        break
    }
}

if ($filesOk) {
    Write-Host " ✓ All present" -ForegroundColor Green
} else {
    Write-Host " ✗ Some files missing" -ForegroundColor Red
    $allGood = $false
}

# Summary
Write-Host "`n========================================" -ForegroundColor Cyan
if ($allGood) {
    Write-Host " ✓ Ready to Build!" -ForegroundColor Green
    Write-Host "========================================`n" -ForegroundColor Cyan
    
    Write-Host "Next steps:" -ForegroundColor White
    Write-Host "1. mkdir build" -ForegroundColor Gray
    Write-Host "2. cd build" -ForegroundColor Gray
    Write-Host "3. cmake .." -ForegroundColor Gray
    Write-Host "4. cmake --build . --config Release`n" -ForegroundColor Gray
} else {
    Write-Host " ✗ Missing Requirements" -ForegroundColor Red
    Write-Host "========================================`n" -ForegroundColor Cyan
    
    Write-Host "Please install missing components and try again." -ForegroundColor Yellow
    Write-Host "See BUILD_GUIDE.md for detailed instructions.`n" -ForegroundColor Yellow
}

Write-Host "For help, check:" -ForegroundColor White
Write-Host "  - QUICK_START.md (beginner guide)" -ForegroundColor Gray
Write-Host "  - BUILD_GUIDE.md (detailed instructions)" -ForegroundColor Gray
Write-Host "  - README.md (full documentation)`n" -ForegroundColor Gray

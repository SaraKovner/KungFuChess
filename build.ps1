Write-Host "Building KFC Merged Project..." -ForegroundColor Green

if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build"
}
Set-Location "build"

# Clean any previous build cache
if (Test-Path "CMakeCache.txt") { Remove-Item "CMakeCache.txt" }
if (Test-Path "CMakeFiles") { Remove-Item "CMakeFiles" -Recurse -Force }

Write-Host "Running CMake configuration..." -ForegroundColor Yellow

# Try Visual Studio 2022 first
Write-Host "Trying Visual Studio 2022..." -ForegroundColor Cyan
cmake .. -G "Visual Studio 17 2022" -A x64 2>$null
if ($LASTEXITCODE -eq 0) {
    Write-Host "Success with Visual Studio 2022" -ForegroundColor Green
} else {
    # Clean and try MinGW
    if (Test-Path "CMakeCache.txt") { Remove-Item "CMakeCache.txt" }
    if (Test-Path "CMakeFiles") { Remove-Item "CMakeFiles" -Recurse -Force }
    
    Write-Host "Trying MinGW Makefiles..." -ForegroundColor Cyan
    cmake .. -G "MinGW Makefiles" 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Success with MinGW" -ForegroundColor Green
    } else {
        # Clean and try default
        if (Test-Path "CMakeCache.txt") { Remove-Item "CMakeCache.txt" }
        if (Test-Path "CMakeFiles") { Remove-Item "CMakeFiles" -Recurse -Force }
        
        Write-Host "Trying default generator..." -ForegroundColor Cyan
        cmake ..
        if ($LASTEXITCODE -ne 0) {
            Write-Host "CMake configuration failed!" -ForegroundColor Red
            Read-Host "Press Enter to exit"
            exit 1
        }
    }
}

Write-Host "Building project..." -ForegroundColor Yellow
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Read-Host "Press Enter to continue"
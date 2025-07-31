Write-Host "Building and running Observer system..." -ForegroundColor Green

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build"
    Write-Host "Build directory created" -ForegroundColor Yellow
}

Set-Location "build"

# Run CMake
Write-Host "Configuring project with CMake..." -ForegroundColor Cyan
cmake .. -G "Visual Studio 17 2022" -A x64

if ($LASTEXITCODE -ne 0) {
    Write-Host "Visual Studio not found, trying MinGW..." -ForegroundColor Yellow
    cmake .. -G "MinGW Makefiles"
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "MinGW not found, trying default compiler..." -ForegroundColor Yellow
    cmake ..
}

# Build
Write-Host "Building project..." -ForegroundColor Cyan
cmake --build . --config Release

if ($LASTEXITCODE -eq 0) {
    Set-Location ..
    Write-Host "Build completed successfully!" -ForegroundColor Green
    
    # Run
    Write-Host "Running Observer demo..." -ForegroundColor Magenta
    
    if (Test-Path "build\Release\observer_demo.exe") {
        & "build\Release\observer_demo.exe"
    } elseif (Test-Path "build\Debug\observer_demo.exe") {
        & "build\Debug\observer_demo.exe" 
    } elseif (Test-Path "build\observer_demo.exe") {
        & "build\observer_demo.exe"
    } else {
        Write-Host "Executable not found!" -ForegroundColor Red
    }
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
}

Write-Host "Press Enter to exit..."
Read-Host

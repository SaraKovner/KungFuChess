Write-Host "Building and running KungFu Chess..." -ForegroundColor Green

# Build first
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build"
}

Set-Location "build"
cmake .. -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) {
    cmake .. -G "MinGW Makefiles"
    if ($LASTEXITCODE -ne 0) {
        cmake ..
    }
}

cmake --build . --config Release

if ($LASTEXITCODE -eq 0) {
    Set-Location ..
    Write-Host "Build successful! Starting game..." -ForegroundColor Green
    
    # Try different executable locations
    if (Test-Path "build\Release\KungFuChess.exe") {
        & "build\Release\KungFuChess.exe"
    } elseif (Test-Path "build\Debug\KungFuChess.exe") {
        & "build\Debug\KungFuChess.exe"
    } elseif (Test-Path "build\KungFuChess.exe") {
        & "build\KungFuChess.exe"
    } else {
        Write-Host "Executable not found!" -ForegroundColor Red
    }
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
}

Write-Host "Press Enter to exit..."
Read-Host
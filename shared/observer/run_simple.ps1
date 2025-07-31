# Simple Observer Demo Runner
Write-Host "Building and running Simple Observer Demo..." -ForegroundColor Green

# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build"
}

# Build
Set-Location "build"
cmake ..
cmake --build . --config Release

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful! Running simple demo..." -ForegroundColor Green
    .\Release\simple_demo.exe
} else {
    Write-Host "Build failed!" -ForegroundColor Red
}

Set-Location ..
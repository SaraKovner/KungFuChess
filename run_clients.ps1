# PowerShell script to run 2 KungFu Chess clients simultaneously

Write-Host "🎮 Starting KungFu Chess Clients..." -ForegroundColor Green

# Get the root directory (where this script is located)
$rootDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$clientExe = Join-Path $rootDir "build\Release\client.exe"

# Check if the client executable exists
if (-not (Test-Path $clientExe)) {
    Write-Host "❌ Client executable not found: $clientExe" -ForegroundColor Red
    exit 1
}

Write-Host "🚀 Starting Client 1 (WHITE)..." -ForegroundColor Yellow
Start-Process -FilePath $clientExe -WorkingDirectory $rootDir

# Longer delay to ensure first client fully initializes
Write-Host "⏳ Waiting for first client to initialize..." -ForegroundColor Cyan
Start-Sleep -Seconds 3

Write-Host "🚀 Starting Client 2 (BLACK)..." -ForegroundColor Yellow  
Start-Process -FilePath $clientExe -WorkingDirectory $rootDir

Write-Host "✅ Both clients started!" -ForegroundColor Green
Write-Host "First client will be WHITE, second will be BLACK" -ForegroundColor Cyan
Write-Host "Make sure the server is running first!" -ForegroundColor Magenta

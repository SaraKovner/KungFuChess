# Quick Test Script for KungFu Chess

Write-Host "🚀 Quick Test - KungFu Chess Network" -ForegroundColor Green

# Build only if needed
if (!(Test-Path "build\Release\KungFuChess_Server.exe") -or !(Test-Path "build\Release\KungFuChess_Client.exe")) {
    Write-Host "🔨 Building project first..." -ForegroundColor Yellow
    
    if (!(Test-Path "build")) {
        New-Item -ItemType Directory -Path "build"
    }
    
    Set-Location build
    cmake .. -G "Visual Studio 16 2019" -A x64
    cmake --build . --config Release
    Set-Location ..
    
    # Copy OpenCV DLL
    Copy-Item "OpenCV_451\bin\opencv_world451.dll" "build\Release\" -Force
}

Write-Host "🎯 Starting Server and Client for testing..." -ForegroundColor Cyan

# Start server in background
Write-Host "🖥️ Starting server..." -ForegroundColor Yellow
$serverProcess = Start-Process -FilePath ".\build\Release\KungFuChess_Server.exe" -PassThru

# Wait for server to initialize
Write-Host "⏳ Waiting for server to start..." -ForegroundColor Yellow
Start-Sleep -Seconds 2

# Start first client
Write-Host "💻 Starting first client..." -ForegroundColor Yellow
$client1Process = Start-Process -FilePath ".\build\Release\KungFuChess_Client.exe" -PassThru

# Wait a bit
Start-Sleep -Seconds 1

# Start second client
Write-Host "💻 Starting second client..." -ForegroundColor Yellow
$client2Process = Start-Process -FilePath ".\build\Release\KungFuChess_Client.exe" -PassThru

Write-Host "✅ Server and 2 clients started!" -ForegroundColor Green
Write-Host "📋 Processes:" -ForegroundColor Cyan
Write-Host "   Server PID: $($serverProcess.Id)" -ForegroundColor White
Write-Host "   Client 1 PID: $($client1Process.Id)" -ForegroundColor White  
Write-Host "   Client 2 PID: $($client2Process.Id)" -ForegroundColor White

Write-Host ""
Write-Host "🎮 Game should be running now!" -ForegroundColor Green
Write-Host "   - Each client will get a different color (White/Black)" -ForegroundColor White
Write-Host "   - Use arrow keys + Enter to play" -ForegroundColor White
Write-Host "   - Press ESC to exit" -ForegroundColor White
Write-Host ""
Write-Host "Press any key to stop all processes..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

# Clean up processes
Write-Host "🧹 Cleaning up processes..." -ForegroundColor Yellow
try { Stop-Process -Id $serverProcess.Id -Force } catch {}
try { Stop-Process -Id $client1Process.Id -Force } catch {}
try { Stop-Process -Id $client2Process.Id -Force } catch {}

Write-Host "✅ Test completed!" -ForegroundColor Green
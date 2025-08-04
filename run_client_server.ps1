# Script to run KungFu Chess Client-Server
Write-Host "🚀 Starting KungFu Chess Client-Server..." -ForegroundColor Green

# Start server in background
Write-Host "🖥️ Starting server..." -ForegroundColor Yellow
Start-Process -FilePath ".\build\Release\KungFuChess_Server.exe" -WindowStyle Normal

# Wait a moment for server to start
Start-Sleep -Seconds 2

# Start first client (WHITE player)
Write-Host "👤 Starting WHITE player client..." -ForegroundColor White
Start-Process -FilePath ".\build\Release\KungFuChess_Client.exe" -WindowStyle Normal

# Wait a moment
Start-Sleep -Seconds 2

# Start second client (BLACK player)
Write-Host "👤 Starting BLACK player client..." -ForegroundColor Black
Start-Process -FilePath ".\build\Release\KungFuChess_Client.exe" -WindowStyle Normal

Write-Host "✅ All processes started!" -ForegroundColor Green
Write-Host "🎮 Server running on port 8080" -ForegroundColor Cyan
Write-Host "🎯 Two client windows should open shortly" -ForegroundColor Cyan
Write-Host "" 
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

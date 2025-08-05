# Build and Run Script for KungFu Chess Network Game

Write-Host "🎮 KungFu Chess - Build and Run Script" -ForegroundColor Green

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build"
}

# Build the project
Write-Host "🔨 Building project..." -ForegroundColor Yellow
Set-Location build
cmake .. -G "Visual Studio 16 2019" -A x64
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

cmake --build . --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Set-Location ..

Write-Host "✅ Build completed successfully!" -ForegroundColor Green

# Copy OpenCV DLL to build directory
Copy-Item "OpenCV_451\bin\opencv_world451.dll" "build\Release\" -Force

Write-Host "📋 Choose an option:" -ForegroundColor Cyan
Write-Host "1. Start Server" -ForegroundColor White
Write-Host "2. Start Client" -ForegroundColor White
Write-Host "3. Start Both (Server + Client)" -ForegroundColor White
Write-Host "4. Local Game (No Network)" -ForegroundColor White

$choice = Read-Host "Enter your choice (1-4)"

switch ($choice) {
    "1" {
        Write-Host "🖥️ Starting Server..." -ForegroundColor Green
        & ".\build\Release\KungFuChess_Server.exe"
    }
    "2" {
        Write-Host "💻 Starting Client..." -ForegroundColor Green
        & ".\build\Release\KungFuChess_Client.exe"
    }
    "3" {
        Write-Host "🌐 Starting Server and Client..." -ForegroundColor Green
        
        # Start server in background
        $serverJob = Start-Job -ScriptBlock {
            Set-Location $using:PWD
            & ".\build\Release\KungFuChess_Server.exe"
        }
        
        Write-Host "⏳ Waiting 3 seconds for server to start..." -ForegroundColor Yellow
        Start-Sleep -Seconds 3
        
        # Start client
        & ".\build\Release\KungFuChess_Client.exe"
        
        # Clean up server job
        Stop-Job $serverJob -Force
        Remove-Job $serverJob -Force
    }
    "4" {
        Write-Host "🏠 Starting Local Game..." -ForegroundColor Green
        & ".\build\Release\KungFuChess_Client.exe" --local
    }
    default {
        Write-Host "❌ Invalid choice!" -ForegroundColor Red
    }
}

Write-Host "🏁 Script completed." -ForegroundColor Green
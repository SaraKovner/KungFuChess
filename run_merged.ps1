# KungFu Chess - Merged Version Runner
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "    KungFu Chess - Merged Version" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Current branch: merge-input-and-movement" -ForegroundColor Green
Write-Host "Features: Numpad controls + Piece movement" -ForegroundColor Green
Write-Host ""
Write-Host "Controls:" -ForegroundColor White
Write-Host "  8 = Up    2 = Down" -ForegroundColor Gray
Write-Host "  4 = Left  6 = Right" -ForegroundColor Gray
Write-Host "  Enter = Select/Move piece" -ForegroundColor Gray
Write-Host "  ESC = Exit" -ForegroundColor Gray
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Change to project directory
Set-Location "c:\Users\This User\Documents\הנדסאות\CTD\איטרציה 2\CTD_Sara\KungFuChess"

# Check current branch
$currentBranch = git branch --show-current
if ($currentBranch -ne "merge-input-and-movement") {
    Write-Host "Switching to merge-input-and-movement branch..." -ForegroundColor Yellow
    git checkout merge-input-and-movement
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Failed to switch to branch!" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

# Build if needed
if (!(Test-Path "build\Release\KungFuChess.exe")) {
    Write-Host "Building project..." -ForegroundColor Yellow
    Set-Location build
    cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Build failed!" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
    Set-Location ..
}

# Run the game
Write-Host "Starting game..." -ForegroundColor Green
Write-Host ""
& "build\Release\KungFuChess.exe"

Write-Host ""
Write-Host "Game ended." -ForegroundColor Yellow
Read-Host "Press Enter to exit"
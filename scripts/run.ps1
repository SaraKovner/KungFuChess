Write-Host "Running KFC Merged Game..." -ForegroundColor Green

# Try different build locations
$buildPaths = @(
    "build\Release\KungFuChess.exe",
    "build\Debug\KungFuChess.exe", 
    "build\KungFuChess.exe"
)

$found = $false
foreach ($path in $buildPaths) {
    if (Test-Path $path) {
        Write-Host "Found executable: $path" -ForegroundColor Cyan
        # Run from project root directory, not build directory
        $exe = Resolve-Path $path
        & $exe
        $found = $true
        break
    }
}

if (!$found) {
    Write-Host "Executable not found!" -ForegroundColor Red
    Write-Host "Please build the project first using: .\build.ps1" -ForegroundColor Yellow
}

Read-Host "Press Enter to exit"
@echo off
echo ========================================
echo    KungFu Chess - Merged Version
echo ========================================
echo.
echo Current branch: merge-input-and-movement
echo Features: Numpad controls + Piece movement
echo.
echo Controls:
echo   8 = Up    2 = Down
echo   4 = Left  6 = Right  
echo   Enter = Select/Move piece
echo   ESC = Exit
echo.
echo ========================================
echo.

cd /d "c:\Users\This User\Documents\הנדסאות\CTD\איטרציה 2\CTD_Sara\KungFuChess"

REM Check if we're on the right branch
git branch --show-current > temp_branch.txt
set /p CURRENT_BRANCH=<temp_branch.txt
del temp_branch.txt

if not "%CURRENT_BRANCH%"=="merge-input-and-movement" (
    echo Switching to merge-input-and-movement branch...
    git checkout merge-input-and-movement
    if errorlevel 1 (
        echo ERROR: Failed to switch to branch!
        pause
        exit /b 1
    )
)

REM Build if needed
if not exist "build\Release\KungFuChess.exe" (
    echo Building project...
    cd build
    cmake --build . --config Release
    if errorlevel 1 (
        echo ERROR: Build failed!
        pause
        exit /b 1
    )
    cd ..
)

REM Run the game
echo Starting game...
echo.
build\Release\KungFuChess.exe

echo.
echo Game ended.
pause
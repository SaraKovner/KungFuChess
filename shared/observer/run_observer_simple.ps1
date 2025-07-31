Write-Host "🎯 בונה ומריץ מערכת Observer מתקדמת..." -ForegroundColor Green

# יצירת תיקיית build
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build"
    Write-Host "✅ תיקיית build נוצרה" -ForegroundColor Yellow
}

Set-Location "build"

# הרצת CMake
Write-Host "🔧 מגדיר פרויקט עם CMake..." -ForegroundColor Cyan
cmake .. -G "Visual Studio 17 2022" -A x64

if ($LASTEXITCODE -ne 0) {
    Write-Host "⚠️ Visual Studio לא נמצא, מנסה MinGW..." -ForegroundColor Yellow
    cmake .. -G "MinGW Makefiles"
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "⚠️ MinGW לא נמצא, מנסה compiler ברירת מחדל..." -ForegroundColor Yellow
    cmake ..
}

# בנייה
Write-Host "🔨 בונה פרויקט..." -ForegroundColor Cyan
cmake --build . --config Release

if ($LASTEXITCODE -eq 0) {
    Set-Location ..
    Write-Host "✅ הבנייה הושלמה בהצלחה!" -ForegroundColor Green
    
    # הרצה
    Write-Host "`n🚀 מריץ דמו Observer..." -ForegroundColor Magenta
    
    if (Test-Path "build\Release\observer_demo.exe") {
        & "build\Release\observer_demo.exe"
    } elseif (Test-Path "build\Debug\observer_demo.exe") {
        & "build\Debug\observer_demo.exe" 
    } elseif (Test-Path "build\observer_demo.exe") {
        & "build\observer_demo.exe"
    } else {
        Write-Host "❌ קובץ הפעלה לא נמצא!" -ForegroundColor Red
    }
} else {
    Write-Host "❌ הבנייה נכשלה!" -ForegroundColor Red
    Set-Location ..
}

Write-Host "`n⏸️ לחץ Enter לסגירה..."
Read-Host

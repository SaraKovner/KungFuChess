# KFC Merged - Kung Fu Chess Game

פרויקט משולב שמשלב את הטוב משני הפרויקטים:
- **KFC_CPP**: מבנה קוד מפותח, מערכת State machines, תמיכה ב-OpenCV
- **CTD25_1**: תמיכה במקביליות, ממשק משתמש משופר

## מבנה הפרויקט המסודר

```
KungFuChess/
├── src/                           # קוד מקור ראשי
│   ├── core/                      # מנוע המשחק הבסיסי
│   │   ├── Game.hpp/cpp          # מנוע המשחק הראשי
│   │   ├── Board.hpp/cpp         # לוח המשחק
│   │   ├── Piece.hpp             # כלי המשחק
│   │   ├── State.hpp             # מצבי הכלים
│   │   ├── Physics.hpp           # פיזיקת המשחק
│   │   └── Common.hpp            # הגדרות משותפות
│   ├── graphics/                  # מערכת גרפיקה
│   │   ├── Graphics.hpp/cpp      # גרפיקה ואנימציות
│   │   ├── GraphicsFactory.hpp   # יצרן גרפיקה
│   │   └── img/                  # מערכת תמונות
│   ├── game_logic/               # לוגיקת המשחק
│   │   ├── Moves.hpp/cpp         # מערכת תנועות
│   │   ├── CaptureRules.hpp/cpp  # חוקי לכידה
│   │   ├── PieceFactory.hpp      # יצרן הכלים
│   │   └── PhysicsFactory.hpp    # יצרן פיזיקה
│   ├── ui/                       # ממשק משתמש
│   │   └── Command.hpp           # מערכת פקודות
│   ├── observer/                 # מערכת Observer Pattern
│   ├── utils/                    # כלי עזר
│   │   └── json/                 # ספריית JSON
│   └── main.cpp                  # נקודת כניסה
├── assets/                       # נכסי המשחק
│   ├── pieces/                   # כלי המשחק
│   └── sounds/                   # קבצי שמע
├── external/                     # ספריות חיצוניות
│   └── OpenCV_451/              # ספריית OpenCV
├── scripts/                      # סקריפטי בנייה והרצה
│   ├── build.ps1
│   └── run.ps1
├── docs/                         # תיעוד
├── tests/                        # בדיקות
├── CMakeLists.txt               # קובץ בנייה ראשי
└── .gitignore                   # Git ignore
```

## תכונות מרכזיות

### מ-KFC_CPP:
- ✅ מערכת State machines מפותחת
- ✅ תמיכה מלאה ב-OpenCV לגרפיקה
- ✅ מבנה קוד מודולרי ונקי
- ✅ מערכת פיזיקה מתקדמת
- ✅ אנימציות ו-sprites

### מ-CTD25_1:
- ✅ תמיכה במקביליות (threads)
- ✅ ממשק משתמש אינטראקטיבי
- ✅ מערכת פקודות משופרת
- ✅ טיפול בקלט משתמש
- ✅ מנגנון בחירה והזזת כלים

### תכונות משולבות:
- 🔄 Game loop מתקדם עם threading
- 🎮 ממשק משתמש אינטראקטיבי
- 🎨 גרפיקה מתקדמת עם אנימציות
- ⚡ ביצועים משופרים
- 🛡️ Thread safety
- 🎯 מערכת collision detection

## דרישות מערכת

- Windows 10/11
- Visual Studio 2019 או חדש יותר
- CMake 3.16+
- OpenCV 4.5.1 (כלול בפרויקט)

## הוראות בנייה והרצה

### בנייה:
```powershell
# הרץ את סקריפט הבנייה מתיקיית scripts
cd scripts
.\build.ps1
```

### הרצה:
```powershell
# הרץ את המשחק מתיקיית scripts
cd scripts
.\run.ps1
```

### בנייה ידנית:
```powershell
New-Item -ItemType Directory -Name "build"
Set-Location "build"
cmake ..
cmake --build . --config Release
```

## שימוש במשחק

### בקרות:
- **עכבר**: לחיצה לבחירת כלי והזזה
- **W/A/S/D**: הזזת סמן
- **Enter**: אישור תנועה
- **ESC**: ביטול בחירה

### מהלך המשחק:
1. המשחק מתחיל עם כלי שחמט סטנדרטיים
2. כל כלי יכול לזוז בהתאם לחוקי השחמט
3. הכלים מתעדכנים בזמן אמת
4. המשחק תומך באנימציות וגרפיקה מתקדמת

## פיתוח נוסף

הפרויקט מוכן להרחבות:
- הוספת AI למחשב
- רשת מרובת משתתפים
- עוד סוגי כלים
- מצבי משחק נוספים
- שיפורי גרפיקה

## פתרון בעיות

### שגיאות בנייה:
- ודא שיש לך Visual Studio מותקן
- בדוק שנתיב OpenCV נכון
- הרץ כ-Administrator אם נדרש

### שגיאות הרצה:
- ודא שתיקיית assets/pieces/ קיימת
- בדוק שקבצי DLL של OpenCV נמצאים בתיקיית ההרצה
- הרץ מתיקיית הפרויקט הראשית

## רישיון

פרויקט לימודי - CTD BootCamp 2025
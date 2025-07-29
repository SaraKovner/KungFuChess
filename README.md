# Kung Fu Chess Game

פרויקט משחק שחמט מתקדם עם מערכת State machines, גרפיקה מבוססת OpenCV ותמיכה במקביליות.

## מבנה הפרויקט

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
│   │   └── img/                  # מערכת תמונות OpenCV
│   │       ├── Img.hpp           # ממשק תמונה
│   │       ├── ImgFactory.hpp    # יצרן תמונות
│   │       ├── MockImg.hpp       # Mock לבדיקות
│   │       └── OpenCvImg.hpp/cpp # יישום OpenCV
│   ├── game_logic/               # לוגיקת המשחק
│   │   ├── Moves.hpp/cpp         # מערכת תנועות
│   │   ├── CaptureRules.hpp/cpp  # חוקי לכידה
│   │   ├── PieceFactory.hpp      # יצרן הכלים
│   │   └── PhysicsFactory.hpp    # יצרן פיזיקה
│   ├── ui/                       # ממשק משתמש
│   │   └── Command.hpp           # מערכת פקודות
│   ├── utils/                    # כלי עזר
│   │   └── json/                 # ספריית JSON (nlohmann)
│   └── main.cpp                  # נקודת כניסה
├── observer/                     # מערכת Observer Pattern
│   ├── headers/                  # כותרות Observer
│   │   ├── GameEventManager.hpp  # מנהל אירועים
│   │   ├── SoundManager.hpp      # מנהל קול
│   │   ├── Observer.hpp          # ממשק Observer
│   │   └── Subject.hpp           # ממשק Subject
│   └── src/                      # יישום Observer
├── assets/                       # נכסי המשחק
│   ├── pieces/                   # כלי המשחק (BB, BW, KB, KW, NB, NW, PB, PW, QB, QW, RB, RW)
│   │   └── [PIECE]/states/       # מצבי הכלים (idle, move, jump, rest)
│   │       ├── sprites/          # תמונות אנימציה
│   │       └── config.json       # הגדרות מצב
│   └── sounds/                   # קבצי שמע
├── external/                     # ספריות חיצוניות
│   └── OpenCV_451/              # ספריית OpenCV
│       ├── bin/                  # DLL files
│       └── include/              # Header files
├── scripts/                      # סקריפטי בנייה והרצה
│   ├── build.ps1                # סקריפט בנייה
│   └── run.ps1                  # סקריפט הרצה
├── tests/                        # בדיקות יחידה
│   ├── test_*.cpp               # קבצי בדיקה
│   └── catch.hpp                # מסגרת בדיקות
├── docs/                         # תיעוד
├── CMakeLists.txt               # קובץ בנייה ראשי
└── .gitignore                   # Git ignore
```

## תכונות מרכזיות

### מערכת State Machines:
- ✅ מצבי כלים מתקדמים (idle, move, jump, short_rest, long_rest)
- ✅ מעברי מצב מבוססי קבצי תצורה
- ✅ אנימציות sprites לכל מצב
- ✅ מערכת transitions מתקדמת

### גרפיקה ואנימציות:
- ✅ תמיכה מלאה ב-OpenCV
- ✅ מערכת תמונות מודולרית
- ✅ אנימציות רציפות לכלים
- ✅ רקע ולוח משחק גרפיים

### מערכת Observer Pattern:
- ✅ מנהל אירועי משחק
- ✅ מערכת קול ואפקטים
- ✅ מעקב אחר תנועות ולכידות
- ✅ הכרזות קוליות

### לוגיקת משחק:
- ✅ חוקי שחמט מלאים
- ✅ מערכת לכידה מתקדמת
- ✅ תמיכה בכל סוגי הכלים
- ✅ פיזיקת משחק מדויקת

### בדיקות ואיכות:
- ✅ מערכת בדיקות יחידה מקיפה
- ✅ Mock objects לבדיקות
- ✅ כיסוי בדיקות רחב

## דרישות מערכת

- Windows 10/11
- Visual Studio 2019 או חדש יותר
- CMake 3.16+
- OpenCV 4.5.1 (כלול בפרויקט)

## הוראות בנייה והרצה

### בנייה:
```powershell
# הרץ את סקריפט הבנייה
.\scripts\build.ps1
```

### הרצה:
```powershell
# הרץ את המשחק
.\scripts\run.ps1

# או הרץ את מערכת ה-Observer
.\observer\run.ps1
```

### בנייה ידנית:
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### הרצת בדיקות:
```powershell
# לאחר בנייה
.\build\Release\kungfu_chess_tests.exe
```

## שימוש במשחק

### בקרות:
- **עכבר**: לחיצה לבחירת כלי והזזה
- **מקלדת**: ניווט בלוח
- **ESC**: יציאה מהמשחק

### מהלך המשחק:
1. המשחק מתחיל עם כלי שחמט סטנדרטיים
2. כל כלי עובר דרך מצבים שונים (idle → move → rest)
3. אנימציות רציפות לכל תנועה
4. מערכת קול ואפקטים
5. מעקב אחר תוצאות המשחק

### כלי המשחק:
- **P** (Pawn) - רגלי
- **R** (Rook) - צריח  
- **N** (Knight) - סוס
- **B** (Bishop) - רץ
- **Q** (Queen) - מלכה
- **K** (King) - מלך
- **W/B** - לבן/שחור

## ארכיטקטורה

### עקרונות עיצוב:
- **Factory Pattern**: יצירת אובייקטים מודולרית
- **Observer Pattern**: מערכת אירועים מבוזרת
- **State Machine**: ניהול מצבי כלים
- **Dependency Injection**: הפרדת תלויות
- **SOLID Principles**: עיצוב מודולרי ונקי

### טכנולוגיות:
- **C++17**: שפת התכנות הראשית
- **OpenCV 4.5.1**: עיבוד תמונות וגרפיקה
- **nlohmann/json**: עיבוד קבצי JSON
- **CMake**: מערכת בנייה
- **Catch2**: מסגרת בדיקות

## פיתוח נוסף

הפרויקט מוכן להרחבות:
- הוספת AI למחשב
- רשת מרובת משתתפים  
- מצבי משחק נוספים (Kung Fu Chess)
- שיפורי גרפיקה ואנימציות
- מערכת שמירה וטעינה

## פתרון בעיות

### שגיאות בנייה:
- ודא שיש לך Visual Studio 2019+ מותקן
- בדוק שנתיב OpenCV נכון ב-CMakeLists.txt
- הרץ כ-Administrator אם נדרש
- ודא ש-CMake 3.16+ מותקן

### שגיאות הרצה:
- ודא שתיקיית assets/pieces/ קיימת עם כל הכלים
- בדוק שקבצי DLL של OpenCV נמצאים בתיקיית ההרצה
- הרץ מתיקיית הפרויקט הראשית
- ודא שקבצי config.json תקינים בתיקיות הכלים

### בדיקת תקינות:
```powershell
# בדוק שהבנייה עברה בהצלחה
ls build/Release/

# הרץ בדיקות
.\build\Release\kungfu_chess_tests.exe

# בדוק נכסים
ls assets/pieces/
```

## מידע נוסף

### קבצי תצורה:
- `assets/pieces/[PIECE]/states/[STATE]/config.json` - הגדרות אנימציה
- `assets/pieces/[PIECE]/states/transitions.csv` - מעברי מצב
- `assets/pieces/board.csv` - הגדרות לוח

### לוגים ודיבוג:
המשחק מדפיס מידע דיבוג לקונסול כולל:
- טעינת נכסים
- מעברי מצב
- אירועי משחק
- שגיאות

## רישיון

פרויקט לימודי - CTD BootCamp 2025
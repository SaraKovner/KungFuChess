# Kung Fu Chess Game

פרויקט משחק שחמט מתקדם עם מערכת State machines, גרפיקה מבוססת OpenCV ותמיכה במקביליות.

## מבנה הפרויקט

```
KungFuChess/
├── shared/                       # Shared code for all components
│   ├── core/                     # Core game engine
│   │   ├── Game.hpp/cpp         # Main game engine
│   │   ├── Board.hpp/cpp        # Game board
│   │   ├── Piece.hpp            # Game pieces
│   │   ├── State.hpp            # Piece states
│   │   ├── Physics.hpp          # Game physics
│   │   └── Common.hpp           # Common definitions
│   ├── graphics/                # Graphics system
│   │   ├── Graphics.hpp/cpp     # Graphics and animations
│   │   ├── GraphicsFactory.hpp  # Graphics factory
│   │   └── img/                 # OpenCV image system
│   ├── game_logic/              # Game logic
│   │   ├── Moves.hpp/cpp        # Movement system
│   │   ├── CaptureRules.hpp/cpp # Capture rules
│   │   ├── PieceFactory.hpp     # Piece factory
│   │   └── PhysicsFactory.hpp   # Physics factory
│   ├── network/                 # Network interface
│   │   └── NetworkInterface.hpp # Network interface
│   ├── observer/                # Observer Pattern system
│   │   ├── headers/             # Observer headers
│   │   ├── src/                 # Observer implementation
│   │   └── run*.ps1             # Run scripts
│   ├── ui/                      # User interface
│   │   └── Command.hpp          # Game commands
│   └── utils/                   # Utilities
│       └── json/                # JSON library
├── server/                      # Game server
│   ├── network/                 # Server network
│   │   ├── GameServer.hpp/cpp   # Game server
│   │   └── GameServerLogic.cpp  # Server logic
│   └── main_server.cpp          # Server entry point
├── client/                      # Game client
│   ├── network/                 # Client network
│   │   └── GameClient.hpp/cpp   # Game client
│   ├── ui/                      # Client UI
│   │   └── Command.hpp          # Client commands
│   └── main_client.cpp          # Client entry point
├── assets/                      # Game assets
│   ├── pieces/                  # Game pieces (sprites)
│   ├── client_assets/           # Client assets
│   ├── sounds/                  # Sound files
│   └── game_rules/              # Game rules
├── OpenCV_451/                  # OpenCV library
│   ├── bin/                     # DLL files
│   └── include/                 # OpenCV headers
├── scripts/                     # Build and run scripts
├── tests/                       # Unit tests
├── docs/                        # Documentation
├── CMakeLists.txt              # Main build file
├── run_clients.ps1             # Run 2 clients
└── run_tests_fixed.ps1         # Run tests
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
- ✅ משחק 2 שחקנים עם סמנים צבעוניים

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

### הרצה - משחק Client-Server:
```powershell
# 1. הרץ שרת (בחלון נפרד)
.\build\Release\server.exe

# 2. הרץ 2 לקוחות (אוטומטית)
.\run_clients.ps1

# או הרץ לקוח יחיד
.\build\Release\client.exe
```

### הרצה - מערכת Observer:
```powershell
# הרץ Observer demo
.\observer\run.ps1

# או Observer פשוט
.\observer\run_simple.ps1
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
# הרץ בדיקות
.\run_tests_fixed.ps1

# או ידנית
.\build\Release\tests.exe
```

## שימוש במשחק

### ארכיטקטורה Client-Server:
- **שרת**: מנהל את לוגיקת המשחק ומתאם בין הלקוחות
- **לקוח**: מציג גרפיקה ומקבל קלט משחקן
- **Observer**: מערכת מעקב ודיווח נפרדת

### בקרות:
- **שחקן 1 (לבן)**: מקשי חצים + Enter - סמן ירוק, בחירה כחולה
- **שחקן 2 (שחור)**: WASD + רווח - סמן אדום, בחירה צהובה
- **ESC**: יציאה מהמשחק

### מהלך המשחק:
1. הרץ שרת ראשון
2. הרץ 2 לקוחות (הראשון יהיה לבן, השני שחור)
3. כל כלי עובר דרך מצבים שונים (idle → move → rest)
4. אנימציות רציפות לכל תנועה
5. מערכת קול ואפקטים
6. מעקב אחר תוצאות המשחק

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

אתגר פיתוח- CTD BootCamp 2025
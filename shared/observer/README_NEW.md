# Observer Pattern - Flexible Architecture

## הארכיטקטורה החדשה

### עקרונות עיצוב:
1. **Observer פשוט** - מקבל BaseEvent ומטפל בסוג האירוע בעצמו
2. **GameEventManager עם Map** - גמישות מקסימלית להוספת אירועים חדשים
3. **Dynamic Cast** - בדיקת סוג האירוע בזמן ריצה

### מבנה הקבצים:
```
observer/
├── headers/
│   ├── BaseEvent.hpp          # בסיס לכל האירועים
│   ├── Observer.hpp           # ממשק Observer פשוט
│   ├── Subject.hpp            # Subject ללא templates
│   ├── GameEventManager.hpp   # מנהל אירועים עם Map
│   ├── CaptureEvent.hpp       # אירוע לכידה
│   ├── MoveEvent.hpp          # אירוע תנועה
│   └── ...
└── src/
    ├── SimpleDemo.cpp         # דוגמה פשוטה
    ├── ObserverDemo.cpp       # דוגמה מלאה
    └── ...
```

## דוגמאות שימוש

### 1. Observer שמטפל באירוע אחד:
```cpp
class WhiteScoreTracker : public Observer {
public:
    void onNotify(const BaseEvent& event) override {
        if (auto* captureEvent = dynamic_cast<const CaptureEvent*>(&event)) {
            handleCapture(*captureEvent);
        }
    }
};
```

### 2. Observer שמטפל במספר אירועים:
```cpp
class VoiceAnnouncer : public Observer {
public:
    void onNotify(const BaseEvent& event) override {
        if (auto* moveEvent = dynamic_cast<const MoveEvent*>(&event)) {
            announceMove(*moveEvent);
        }
        else if (auto* captureEvent = dynamic_cast<const CaptureEvent*>(&event)) {
            announceCapture(*captureEvent);
        }
    }
};
```

### 3. רישום גמיש:
```cpp
GameEventManager eventManager;
VoiceAnnouncer announcer;

// רישום לכמה סוגי אירועים
eventManager.subscribe(&announcer, "move");
eventManager.subscribe(&announcer, "capture");
eventManager.subscribe(&announcer, "gamestate");

// פרסום אירועים
MoveEvent move("P", "e2", "e4", true);
eventManager.publish(move, "move");

CaptureEvent capture("Q", "P", "e4", true, 1);
eventManager.publish(capture, "capture");
```

### 4. הוספת אירוע חדש:
```cpp
// אירוע חדש
class PowerUpEvent : public BaseEvent {
    std::string powerType;
    bool isWhite;
};

// פרסום
PowerUpEvent powerUp;
eventManager.publish(powerUp, "powerup");

// Observer יכול להירשם
eventManager.subscribe(&powerUpHandler, "powerup");
```

## יתרונות הארכיטקטורה:

### 1. גמישות מקסימלית
- קל להוסיף סוגי אירועים חדשים
- Observer יכול להאזין למספר סוגי אירועים
- רישום דינמי לאירועים

### 2. פשטות
- אין צורך ב-templates מסובכים
- ממשק אחיד לכל ה-observers
- קוד נקי וקריא

### 3. הרחבה קלה
- הוספת אירועים חדשים ללא שינוי קוד קיים
- Observer חדש יכול להירשם לכל אירוע
- מערכת מודולרית

## בנייה והרצה:

```bash
# בנייה
mkdir build && cd build
cmake ..
cmake --build . --config Release

# הרצה
./simple_demo.exe      # דוגמה פשוטה
./observer_demo.exe    # דוגמה מלאה
```

## השוואה לארכיטקטורה הקודמת:

| קודם | עכשיו |
|------|-------|
| `Observer<T>` | `Observer` |
| `Subject<T>` | `Subject` |
| רישום נפרד לכל סוג | רישום עם string |
| Templates מסובכים | Dynamic cast פשוט |
| קשיח | גמיש |

הארכיטקטורה החדשה מספקת את כל היתרונות של Observer Pattern עם גמישות מקסימלית ופשטות בשימוש.
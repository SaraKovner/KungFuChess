# 🎯 KungFu Chess - מערכת Observer מתקדמת

שילוב מושלם בין מקצועיות לפשטות!

## 🌟 מה זה?

מערכת Observer מתקדמת שמשלבת:
- **את המקצועיות** של PubSub Pattern (templates, type safety)
- **את הפשטות** של It2_interfaces (קל לשימוש)
- **פיצ'רים ספציפיים** למשחק שח (ניקוד, מהלכים, קולות)

## 📁 מבנה הפרויקט

```
observer/
├── headers/
│   ├── Observer.hpp              # בסיס Observer עם template
│   ├── Subject.hpp               # בסיס Subject עם template
│   ├── GameEventManager.hpp      # מנהל מרכזי לכל האירועים
│   │
│   ├── MoveEvent.hpp             # אירועי תנועת כלים
│   ├── CaptureEvent.hpp          # אירועי אכילת כלים
│   ├── GameStateEvent.hpp        # אירועי שינוי מצב משחק
│   ├── TimeEvent.hpp             # אירועי עדכון זמן
│   │
│   ├── WhiteScoreTracker.hpp     # מעקב ניקוד לבן
│   ├── BlackScoreTracker.hpp     # מעקב ניקוד שחור
│   ├── WhiteMovesTable.hpp       # טבלת מהלכים לבן
│   ├── BlackMovesTable.hpp       # טבלת מהלכים שחור
│   └── VoiceAnnouncer.hpp        # מערכת הכרזות
│
├── src/
│   └── ObserverDemo.cpp          # דמו מלא של המערכת
│
├── CMakeLists.txt                # קובץ build
└── README.md                     # המדריך הזה
```

## 🚀 איך לבנות ולהריץ

### אופציה 1: עם CMake
```bash
cd observer
mkdir build
cd build
cmake ..
cmake --build . --config Release
./observer_demo
```

### אופציה 2: קומפילציה ישירה
```bash
cd observer
g++ -std=c++17 -I headers src/ObserverDemo.cpp -o observer_demo
./observer_demo
```

## 🎯 המטרה

ליצור מערכת שהיא:

1. **מקצועית** - templates, type safety, RAII
2. **פשוטה** - קל לשימוש ולהבנה
3. **ספציפית** - מותאמת למשחק שח
4. **גמישה** - קל להרחיב ולשנות
5. **נעימה** - הודעות בעברית ואמוג'ים

## 💡 איך זה עובד?

1. **GameEventManager** - מנהל מרכזי עם 4 נושאים נפרדים
2. **כל מאזין** יכול להירשם לאירועים שמעניינים אותו
3. **Type Safety** - לכל סוג אירוע יש template נפרד
4. **פשטות** - ממשק נקי ומובן
5. **ביצועים** - רק המאזינים הרלוונטיים מקבלים הודעות

## 🎮 דוגמה לשימוש

```cpp
// יצירת מנהל האירועים
GameEventManager eventManager;

// יצירת מאזינים
WhiteScoreTracker whiteScore;
VoiceAnnouncer announcer;

// רישום לאירועים
eventManager.subscribeToCaptures(&whiteScore);
eventManager.subscribeToMoves(&announcer);

// שליחת אירוע
eventManager.publishMove(MoveEvent("P", "e2", "e4", true));
eventManager.publishCapture(CaptureEvent("Q", "P", "e4", true, 1));
```

## ✨ היתרונות

- **Type Safe**: כל סוג אירוע הוא template נפרד
- **יעיל**: רק מאזינים רלוונטיים מקבלים הודעות  
- **גמיש**: קל להוסיף סוגי אירועים חדשים
- **נקי**: קוד מובן וקריא
- **מלא**: כולל כל מה שצריך למשחק שח

## 🔧 להרחבה

להוספת סוג אירוע חדש:
1. צור struct חדש עבור האירוע
2. הוסף Subject חדש ל-GameEventManager
3. צור Observer שמאזין לאירוע החדש
4. זהו זה!

---

**🏆 שילוב מושלם של מקצועיות ופשטות! 🏆**

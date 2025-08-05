#pragma once

#include "../game_logic/Moves.hpp"
#include "../graphics/Graphics.hpp"
#include "Physics.hpp"
#include <unordered_map>
#include <memory>
#include <string>
#include <cctype>

/**
 * מחלקת מצב כלי - מייצגת מצב ספציפי של כלי במשחק
 * כל מצב כולל תנועה, גרפיקה ופיזיקה
 */
class State : public std::enable_shared_from_this<State> {
public:
    // בנאי - מקבל מרכיבי המצב: תנועה, גרפיקה ופיזיקה
    State(std::shared_ptr<Moves> moves,
          std::shared_ptr<Graphics> graphics,
          std::shared_ptr<BasePhysics> physics)
        : moves(moves), graphics(graphics), physics(physics) {}

    std::shared_ptr<Moves>    moves;      // מרכיב תנועה - מגדיר איך הכלי יכול לזוז
    std::shared_ptr<Graphics> graphics;   // מרכיב גרפי - מטפל באנימציות ותצוגה
    std::shared_ptr<BasePhysics> physics; // מרכיב פיזיקלי - מטפל במיקום וזמן

    // שמירת הפניות חזקות כדי שמצבי יעד לא יושמדו
    std::unordered_map<std::string, std::shared_ptr<State>> transitions; // מעברי מצב
    std::string name; // שם המצב

    // הגדרת מעבר מצב - מאירוע למצב יעד
    void set_transition(const std::string& event, const std::shared_ptr<State>& target) { transitions[event] = target; }

    // איפוס המצב - מאפס את הפיזיקה והגרפיקה
    void reset(const Command& cmd) {
        physics->reset(cmd);
        graphics->reset(cmd);
    }

    // טיפול בפקודה - בודק אם יש מעבר מצב מתאים
    std::shared_ptr<State> on_command(const Command& cmd) {
        std::string key = cmd.type;
        // המרת אותיות לקטנות להתאמה
        for(auto& ch : key) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        auto it = transitions.find(key);
        if(it != transitions.end()) {
            auto next = it->second;
            if(next) {
                next->reset(cmd); // איפוס המצב החדש
                return next;
            }
        }
        return shared_from_this(); // אם אין מעבר, נשאר במצב נוכחי
    }

    // עדכון מצב לפי הזמן - בודק אם יש אירוע פנימי
    std::shared_ptr<State> update(int now_ms) {
        auto internal = physics->update(now_ms);
        if(internal) {
            return on_command(*internal); // אירוע פנימי גרם למעבר מצב
        }
        graphics->update(now_ms); // שמירת הגרפיקה מסונכרנת גם כשאין שינוי מצב
        return shared_from_this();
    }

    // בדיקות לכידה - אם הכלי יכול להילכד או לכוד
    bool can_be_captured() const { return physics->can_be_captured(); }
    bool can_capture()    const { return physics->can_capture(); }
};
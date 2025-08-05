#pragma once

#include <string>
#include <vector>
#include <ostream>

/**
 * מבנה פקודה - מייצג פעולה במשחק (תנועה, קפיצה, סיום וכו')
 * כולל מידע על הזמן, הכלי, סוג הפעולה ופרמטרים
 */
struct Command {
    int timestamp;                 // זמן במילישניות מתחילת המשחק
    std::string piece_id;          // מזהה הכלי (יכול להיות ריק)
    std::string type;              // סוג הפעולה: "move", "jump", "done" וכו'
    std::vector<std::pair<int,int>> params;  // פרמטרים - תאי לוח וכו'
    int player_id = 1;             // מזהה שחקן (1 או 2)

    // בנאי - יוצר פקודה עם כל הפרטים הנדרשים
    Command(int ts, std::string pid, std::string t, std::vector<std::pair<int,int>> p, int player = 1)
        : timestamp(ts), piece_id(pid), type(t), params(p), player_id(player) {}

    // אופרטור הדפסה - להדפסת הפקודה בצורה קריאה
    friend std::ostream& operator<<(std::ostream& os, const Command& cmd) {
        os << "Command(timestamp=" << cmd.timestamp;
        os << ", piece_id=" << cmd.piece_id ;
        os << ", type=" << cmd.type ;
        os << ", params_size=" << cmd.params.size();

        // הדפסת כל הפרמטרים
        for (const auto& param : cmd.params) {
            os << ", {" << param.first << ":" << param.second << "}";
        }
        
        os << ")";
        return os;
    }
};
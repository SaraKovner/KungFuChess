#pragma once

#include "Board.hpp"
#include "../ui/Command.hpp"
#include "Common.hpp"
#include <cmath>
#include <memory>
#include <iostream>

/**
 * מחלקת בסיס לפיזיקה - מגדירה את ההתנהגות הפיזיקלית של כלי
 * כוללת מיקום, תנועה ואינטראקציות עם כלים אחרים
 */
class BasePhysics {
public:
    // בנאי - מקבל לוח ופרמטר (מהירות, משך זמן וכו')
    explicit BasePhysics(const Board& board, double param = 1.0)
        : board(board), param(param) {}

    virtual ~BasePhysics() = default;

    // פונקציות וירטואליות שחייבות להיות ממושות במחלקות הנגזרות
    virtual void reset(const Command& cmd) = 0;  // איפוס הפיזיקה לפי פקודה
    // עדכון מצב הפיזיקה. מחזיר פקודה אם נוצרה, אחרת nullptr
    virtual std::shared_ptr<Command> update(int now_ms) = 0;

    // פונקציות גישה למיקום ומידע
    std::pair<double,double> get_pos_m() const { return curr_pos_m; } // מיקום במטרים
    std::pair<int,int> get_pos_pix() const {  // מיקום בפיקסלים
        double x_m = curr_pos_m.first;
        double y_m = curr_pos_m.second;
        
        // שימוש בערכי פיקסל בטוחים אם הלוח פגום
        int safe_cell_W_pix = (board.cell_W_pix <= 0) ? 80 : board.cell_W_pix;
        int safe_cell_H_pix = (board.cell_H_pix <= 0) ? 80 : board.cell_H_pix;
        
        int x_px = static_cast<int>(std::round(x_m * safe_cell_W_pix));
        int y_px = static_cast<int>(std::round(y_m * safe_cell_H_pix));
        
        return {x_px, y_px};
    }
    std::pair<int,int> get_curr_cell() const { return board.m_to_cell(curr_pos_m); } // תא נוכחי

    int get_start_ms() const { return start_ms; } // זמן התחלה

    // פונקציות בדיקה לאינטראקציות עם כלים אחרים
    virtual bool can_be_captured() const { return true; }     // האם הכלי יכול להילכד
    virtual bool can_capture() const { return true; }        // האם הכלי יכול לכוד
    virtual bool is_movement_blocker() const { return false; } // האם הכלי חוסם תנועה

public:
    const Board& board;                        // הפניה ללוח המשחק
    double param{1.0};                         // פרמטר כללי (מהירות/משך זמן)

    std::pair<int,int> start_cell{0,0};        // תא התחלה
    std::pair<int,int> end_cell{0,0};          // תא יעד
    std::pair<double,double> curr_pos_m{0.0,0.0}; // מיקום נוכחי במטרים
    int start_ms{0};                           // זמן התחלה במילישניות
};

// ---------------------------------------------------------------------------
/**
 * פיזיקת מצב מנוחה - כלי עומד במקום ולא זז
 * הכלי חוסם תנועה ולא יכול לכוד
 */
class IdlePhysics : public BasePhysics {
public:
    using BasePhysics::BasePhysics;
    void reset(const Command& cmd) override {
        if(!cmd.params.empty()) {
            start_cell = end_cell = cmd.params[0];
            curr_pos_m = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        } else {
            // חזרה לנקודת המוצא אם אין פרמטרים
            start_cell = end_cell = {0, 0};
            curr_pos_m = {0.0, 0.0};
        }
        start_ms = cmd.timestamp;
    }
    std::shared_ptr<Command> update(int) override { return nullptr; } // אין עדכון - מצב סטטי

    bool can_capture() const override { return false; }      // לא יכול לכוד במנוחה
    bool is_movement_blocker() const override { return true; } // חוסם תנועה
};

// ---------------------------------------------------------------------------
/**
 * פיזיקת תנועה - מטפלת בתנועה רציפה מתא לתא
 * מחשבת מהירות, מרחק ומשך זמן התנועה
 */
class MovePhysics : public BasePhysics {
public:
    // בנאי - מקבל לוח ומהירות בתאים לשנייה
    explicit MovePhysics(const Board& board, double speed_cells_per_s)
        : BasePhysics(board, speed_cells_per_s) {}

    void reset(const Command& cmd) override {
        if (cmd.params.size() < 2) {
            // פקודה לא חוקית, נשאר במיקום נוכחי
            std::cout << "MOVE: Invalid command - not enough parameters" << std::endl;
            return;
        }
        
        start_cell = cmd.params[0];  // תא התחלה
        end_cell   = cmd.params[1];  // תא יעד
        curr_pos_m = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        start_ms   = cmd.timestamp;

        // חישוב וקטור התנועה ואורכו
        std::pair<double,double> start_pos = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        std::pair<double,double> end_pos = {static_cast<double>(end_cell.second), static_cast<double>(end_cell.first)};
        movement_vec = { end_pos.first - start_pos.first, end_pos.second - start_pos.second };
        movement_len = std::hypot(movement_vec.first, movement_vec.second);
        
        // וידוא שיש לנו מהירות חוקית
        double speed_m_s = (param > 0.0) ? param : 0.5; // ברירת מחדל 0.5 אם לא חוקי
        
        // מניעת חילוק באפס
        if (movement_len > 0.0 && speed_m_s > 0.0) {
            duration_s = movement_len / speed_m_s;
        } else {
            duration_s = 0.1; // משך זמן מינימלי
        }
        
        std::cout << "MOVE: (" << start_cell.first << "," << start_cell.second 
                  << ") -> (" << end_cell.first << "," << end_cell.second << ") duration: " << duration_s << "s" << std::endl;
    }

    std::shared_ptr<Command> update(int now_ms) override {
        // שימוש בערכי לוח בטוחים אם נדרש
        double safe_cell_H_m = (board.cell_H_m < 0.1) ? 1.0 : board.cell_H_m;
        double safe_cell_W_m = (board.cell_W_m < 0.1) ? 1.0 : board.cell_W_m;
        
        double seconds = (now_ms - start_ms) / 1000.0; // המרה לשניות
        if(seconds >= duration_s) {
            // התנועה הסתיימה - הגענו ליעד
            curr_pos_m = {static_cast<double>(end_cell.second), static_cast<double>(end_cell.first)};
            std::cout << "MOVE completed at: (" << curr_pos_m.first << "," << curr_pos_m.second << ")" << std::endl;
            return std::make_shared<Command>(Command{now_ms, "", "done", {end_cell}});
        }
        double ratio = seconds / duration_s; // יחס ההתקדמות
        
        // חישוב בטוח עם בדיקת גבולות
        if (ratio >= 0.0 && ratio <= 1.0 && duration_s > 0.0) {
            auto start_pos = std::make_pair(static_cast<double>(start_cell.second), static_cast<double>(start_cell.first));
            curr_pos_m = { start_pos.first + movement_vec.first * ratio,
                           start_pos.second + movement_vec.second * ratio };
        } else {
            // חזרה למיקום התחלה אם החישוב לא חוקי
            curr_pos_m = std::make_pair(static_cast<double>(start_cell.second), static_cast<double>(start_cell.first));
        }
        return nullptr;
    }

private:
    std::pair<double,double> movement_vec{0.f,0.f}; // וקטור התנועה
    double movement_len{0};                         // אורך התנועה
    double duration_s{1.0};                         // משך התנועה בשניות
public:
    double get_speed_m_s() const { return param; }     // מהירות במטר/שנייה
    double get_duration_s() const { return duration_s; } // משך זמן בשניות
}; // end MovePhysics

// ---------------------------------------------------------------------------
/**
 * פיזיקה סטטית זמנית - כלי עומד במקום למשך זמן מוגדר
 * משמש כמחלקת בסיס למצבי קפיצה ומנוחה
 */
class StaticTemporaryPhysics : public BasePhysics {
public:
    using BasePhysics::BasePhysics;
    double get_duration_s() const { return param; } // משך הזמן בשניות

    void reset(const Command& cmd) override {
        if(!cmd.params.empty()) {
            start_cell = end_cell = cmd.params[0];
            curr_pos_m = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        }
        start_ms = cmd.timestamp;
    }

    std::shared_ptr<Command> update(int now_ms) override {
        double seconds = (now_ms - start_ms) / 1000.0;
        if(seconds >= param) { // אם עבר הזמן הנדרש
            return std::make_shared<Command>(Command{now_ms, "", "done", {end_cell}});
        }
        return nullptr;
    }

    bool is_movement_blocker() const override { return true; } // חוסם תנועה
};

/**
 * פיזיקת קפיצה - כלי לא יכול להילכד במצב זה
 */
class JumpPhysics : public StaticTemporaryPhysics {
public:
    using StaticTemporaryPhysics::StaticTemporaryPhysics;
    bool can_be_captured() const override { return false; } // לא ניתן לכידה
};

/**
 * פיזיקת מנוחה - כלי לא יכול לכוד במצב זה
 */
class RestPhysics : public StaticTemporaryPhysics {
public:
    using StaticTemporaryPhysics::StaticTemporaryPhysics;
    bool can_capture() const override { return false; } // לא יכול לכוד
};

#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <utility>
#include "../core/Common.hpp"

/**
 * מחלקת תנועות - מגדירה את חוקי התנועה של כל סוג כלי
 * טוענת קובץ טקסט עם תנועות יחסיות ומאמתת אותן
 */
class Moves {
public:
    // מבנה תנועה יחסית: dr,dc,tag כאשר tag: -1 שניהם; 0 ללא לכידה; 1 לכידה
    struct RelMove { int dr; int dc; int tag; };

    // בנאי - מקבל נתיב לקובץ תנועות ומימדי הלוח
    Moves(const std::string& txt_path, std::pair<int,int> board_dims);

    // בדיקה אם תא יעד חוקי לפי התנועה היחסית
    bool is_dst_cell_valid(int dr, int dc, bool dst_has_piece) const;
    
    // בדיקה מלאה של חוקיות התנועה כולל נתיב פנוי
    bool is_valid(const std::pair<int,int>& src_cell,
                  const std::pair<int,int>& dst_cell,
                  const std::unordered_set<std::pair<int,int>, PairHash>& cell_with_piece) const;

private:
    std::vector<RelMove> rel_moves; // רשימת תנועות יחסיות אפשריות
    int W; int H;               // רוחב וגובה של הלוח

    // פיענוח שורה מקובץ התנועות
    static RelMove parse_line(const std::string& s);

    // בדיקה שהנתיב בין תא מקור ליעד פנוי מכלים
    bool path_is_clear(const std::pair<int,int>& src_cell,
                       const std::pair<int,int>& dst_cell,
                       const std::unordered_set<std::pair<int,int>, PairHash>& cell_with_piece) const;
};
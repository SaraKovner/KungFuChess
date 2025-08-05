#pragma once

#include "../graphics/img/Img.hpp"
#include <utility>

/**
 * מחלקת לוח המשחק - מייצגת את לוח השחמט ומנהלת המרות קואורדינטות
 * כוללת מידע על גודל תאים, מימדי לוח ותמונת רקע
 */
class Board {
public:
    int cell_H_pix;    // גובה תא בפיקסלים
    int cell_W_pix;    // רוחב תא בפיקסלים
    int W_cells;       // מספר תאים ברוחב (עמודות)
    int H_cells;       // מספר תאים בגובה (שורות)

    ImgPtr img;        // תמונת הלוח
    double cell_H_m;   // גובה תא במטרים (לחישובי פיזיקה)
    double cell_W_m;   // רוחב תא במטרים

public:
    // בנאי - מקבל כל הפרמטרים הנדרשים לאיניציאליזציה
    Board(int cell_H_pix,      // גובה תא בפיקסלים
          int cell_W_pix,      // רוחב תא בפיקסלים
          int W_cells,         // מספר עמודות
          int H_cells,         // מספר שורות
          const ImgPtr& image, // תמונת הלוח
          double cell_H_m = 1.0f, // גובה תא במטרים
          double cell_W_m = 1.0f); // רוחב תא במטרים

    // בנאים ואופרטורים סטנדרטיים
    Board(const Board&) = default;            // בנאי העתקה
    Board(Board&&) noexcept = default;        // בנאי העברה
    Board& operator=(const Board&) = default; // אופרטור השמה בהעתקה
    Board& operator=(Board&&) noexcept = default; // אופרטור השמה בהעברה
    ~Board() = default;                       // הרס ברירת מחדל

    Board clone() const;     // שכפול עמוק של מחזיק התמונה
    void show() const;       // הצגה רק אם תמונה נטענה

    // המרות קואורדינטות -------------------------------------------------
    std::pair<int, int> m_to_cell(const std::pair<double, double>& pos_m) const;      // ממטרים לתא
    std::pair<double, double> cell_to_m(const std::pair<int, int>& cell) const;       // מתא למטרים
    std::pair<int, int> m_to_pix(const std::pair<double, double>& pos_m) const;       // ממטרים לפיקסלים

};
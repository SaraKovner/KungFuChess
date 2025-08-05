#pragma once

// הכללת ספריות סטנדרטיות נדרשות
#include <utility>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// פונקציית hash עבור זוגות מספרים (לשימוש במיקומי לוח)
struct PairHash {
    size_t operator()(const std::pair<int,int>& p) const noexcept {
        // חישוב hash יעיל עבור זוג קואורדינטות (x,y)
        return static_cast<size_t>(p.first) * 31u + static_cast<size_t>(p.second);
    }
};

// הכרזות מוקדמות של מחלקות המשחק
struct Command;

// מחלקת כלי שחמט
class Piece;
using PiecePtr = std::shared_ptr<Piece>;

// מפעל ליצירת תמונות
class ImgFactory;
using ImgFactoryPtr = std::shared_ptr<ImgFactory>;

// מחלקת תמונה
class Img;
using ImgPtr = std::shared_ptr<Img>;

// מחלקת גרפיקה
class Graphics;
using GraphicsPtr = std::shared_ptr<Graphics>;

// מחלקת מצב כלי
class State;
using StatePtr = std::shared_ptr<State>;
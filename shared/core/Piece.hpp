#pragma once

#include "State.hpp"
#include "../ui/Command.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include "Common.hpp"
#include <iostream>

// הכרזה מוקדמת של מחלקת כלי השחמט
class Piece;
typedef std::shared_ptr<Piece> PiecePtr;

/**
 * מחלקת כלי שחמט - מייצגת כלי במשחק
 * כל כלי יש לו מזהה ייחודי ומצב נוכחי
 */
class Piece {
public:
	// בנאי - מקבל מזהה ומצב ראשוני
	Piece(std::string id, std::shared_ptr<State> init_state)
		: id(id), state(init_state) {}

	std::string id;                    // מזהה ייחודי של הכלי (למשל: "PW1", "KB1")
	std::shared_ptr<State> state;      // מצב נוכחי של הכלי (idle, move, rest וכו')

	// הגדרות טיפוסים עבור תא ומיפוי כלים
	using Cell = std::pair<int, int>;  // תא בלוח (שורה, עמודה)
	using Cell2Pieces = std::unordered_map<Cell, std::vector<PiecePtr>, PairHash>; // מיפוי תאים לכלים

	// טיפול בפקודה מהשחקן - מעדכן את מצב הכלי
	void on_command(const Command& cmd, Cell2Pieces&) {
		state = state->on_command(cmd);
	}

	// איפוס הכלי למצב ראשוני (Idle)
	void reset(int start_ms) {
		auto cell = this->current_cell();
		Command cmd{ start_ms,id,"Idle",{cell} };
		state->reset(cmd);
	}

	// עדכון מצב הכלי לפי הזמן הנוכחי
	void update(int now_ms) {
		state = state->update(now_ms);
	}

	// בדיקה אם הכלי חוסם תנועה של כלים אחרים
	bool is_movement_blocker() const { return state->physics->is_movement_blocker(); }

	// החזרת התא הנוכחי של הכלי
	Cell current_cell() const { 
		auto cell = state->physics->get_curr_cell();
		// בדיקת ערכים לא חוקיים ושימוש בערך ברירת מחדל
		if (cell.first < -1000000 || cell.second < -1000000) {
			return state->physics->start_cell;
		}
		return cell;
	}
};
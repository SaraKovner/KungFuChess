#pragma once

#include "State.hpp"
#include "Command.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include "Common.hpp"
#include <iostream>

class Piece;
typedef std::shared_ptr<Piece> PiecePtr;

class Piece {
public:
	Piece(std::string id, std::shared_ptr<State> init_state)
		: id(id), state(init_state) {}

	std::string id;
	std::shared_ptr<State> state;

	using Cell = std::pair<int, int>;
	using Cell2Pieces = std::unordered_map<Cell, std::vector<PiecePtr>, PairHash>;

	void on_command(const Command& cmd, Cell2Pieces&) {
		state = state->on_command(cmd);
	}

	void reset(int start_ms) {
		auto cell = this->current_cell();
		Command cmd{ start_ms,id,"Idle",{cell} };
		state->reset(cmd);
	}

	void update(int now_ms) {
		state = state->update(now_ms);
	}

	bool is_movement_blocker() const { return state->physics->is_movement_blocker(); }

	Cell current_cell() const { 
		auto cell = state->physics->get_curr_cell();
		// Check for invalid values and use fallback
		if (cell.first < -1000000 || cell.second < -1000000) {
			return state->physics->start_cell;
		}
		return cell;
	}
};
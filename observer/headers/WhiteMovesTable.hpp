#pragma once
#include "Observer.hpp"
#include "MoveEvent.hpp"
#include <vector>

/**
 * Move table for white player
 * Listens to moves and maintains complete history
 */
class WhiteMovesTable : public Observer<MoveEvent> {
private:
    std::vector<MoveEvent> moves_;
    
public:
    void onNotify(const MoveEvent& event) override;
    void printAllMoves() const;
    
    size_t getMoveCount() const;
    const std::vector<MoveEvent>& getAllMoves() const;
    void clearMoves();
};

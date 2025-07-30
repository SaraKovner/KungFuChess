#pragma once
#include "Observer.hpp"
#include "MoveEvent.hpp"
#include "BaseEvent.hpp"
#include <vector>

/**
 * Move table for white player
 * Listens to moves and maintains complete history
 */
class WhiteMovesTable : public Observer {
private:
    std::vector<MoveEvent> moves_;
    void handleMove(const MoveEvent& event);
    
public:
    void onNotify(const BaseEvent& event) override;
    void printAllMoves() const;
    
    size_t getMoveCount() const;
    const std::vector<MoveEvent>& getAllMoves() const;
    void clearMoves();
};

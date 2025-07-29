#include "../headers/WhiteMovesTable.hpp"
#include <iostream>
#include <iomanip>

void WhiteMovesTable::onNotify(const MoveEvent& event) {
    // Only white moves
    if (event.isWhite) {
        moves_.push_back(event);
        
        std::cout << "White: " << event.piece 
                  << " " << event.from << "->" << event.to 
                  << " (move #" << moves_.size() << ")" << std::endl;
    }
}

void WhiteMovesTable::printAllMoves() const {
    std::cout << "\n=== White Moves ===" << std::endl;
    for (size_t i = 0; i < moves_.size(); ++i) {
        const auto& move = moves_[i];
        std::cout << std::setw(3) << (i + 1) << ". " 
                  << move.piece << " " << move.from << "->" << move.to;
        if (move.timestamp > 0) {
            std::cout << " (" << std::fixed << std::setprecision(1) 
                      << move.timestamp << "s)";
        }
        std::cout << std::endl;
    }
}

size_t WhiteMovesTable::getMoveCount() const {
    return moves_.size();
}

const std::vector<MoveEvent>& WhiteMovesTable::getAllMoves() const {
    return moves_;
}

void WhiteMovesTable::clearMoves() {
    moves_.clear();
}

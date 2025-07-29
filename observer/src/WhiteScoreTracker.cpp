#include "../headers/WhiteScoreTracker.hpp"
#include <iostream>

WhiteScoreTracker::WhiteScoreTracker() : score_(0) {}

int WhiteScoreTracker::getPieceValue(const std::string& piece) const {
    if (piece == "P") return 1;      // Pawn
    if (piece == "N") return 3;      // Knight
    if (piece == "B") return 3;      // Bishop
    if (piece == "R") return 5;      // Rook
    if (piece == "Q") return 9;      // Queen
    if (piece == "K") return 100;    // King (victory)
    return 0;
}

void WhiteScoreTracker::onNotify(const CaptureEvent& event) {
    // If white captured black piece - add points
    if (event.capturerIsWhite && !event.capturedPiece.empty()) {
        int points = event.scoreValue;
        score_ += points;
        
        std::cout << "Trophy: White captured " << event.capturedPiece 
                  << " at " << event.position 
                  << " (++" << points << " points, total: " << score_ << ")" << std::endl;
    }
    // If black captured white piece - subtract points
    else if (!event.capturerIsWhite && !event.capturingPiece.empty()) {
        int points = event.scoreValue;
        score_ -= points;
        
        std::cout << "Loss: Black captured white piece at " << event.position 
                  << " (--" << points << " points, total: " << score_ << ")" << std::endl;
    }
}

int WhiteScoreTracker::getScore() const {
    return score_;
}

void WhiteScoreTracker::resetScore() {
    score_ = 0;
}

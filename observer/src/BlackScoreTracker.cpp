#include "../headers/BlackScoreTracker.hpp"
#include <iostream>

BlackScoreTracker::BlackScoreTracker() : score_(0) {}

int BlackScoreTracker::getPieceValue(const std::string& piece) const {
    if (piece == "P") return 1;      // Pawn
    if (piece == "N") return 3;      // Knight
    if (piece == "B") return 3;      // Bishop
    if (piece == "R") return 5;      // Rook
    if (piece == "Q") return 9;      // Queen
    if (piece == "K") return 100;    // King (victory)
    return 0;
}

void BlackScoreTracker::onNotify(const CaptureEvent& event) {
    // If black captured white piece - add points
    if (!event.capturerIsWhite && !event.capturedPiece.empty()) {
        int points = getPieceValue(event.capturedPiece);
        score_ += points;
        
        std::cout << "Black: Black captured " << event.capturedPiece 
                  << " at " << event.position 
                  << " (++" << points << " points, total: " << score_ << ")" << std::endl;
    }
    // If white captured black piece - subtract points
    else if (event.capturerIsWhite && !event.capturingPiece.empty()) {
        int points = getPieceValue(event.capturingPiece);
        score_ -= points;
        
        std::cout << "Loss: White captured black piece at " << event.position 
                  << " (--" << points << " points, total: " << score_ << ")" << std::endl;
    }
}

int BlackScoreTracker::getScore() const {
    return score_;
}

void BlackScoreTracker::resetScore() {
    score_ = 0;
}

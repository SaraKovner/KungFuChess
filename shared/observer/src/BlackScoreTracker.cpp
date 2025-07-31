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

void BlackScoreTracker::onNotify(const BaseEvent& event) {
    if (auto* captureEvent = dynamic_cast<const CaptureEvent*>(&event)) {
        handleCapture(*captureEvent);
    }
}

void BlackScoreTracker::handleCapture(const CaptureEvent& event) {
    // If black captured white piece - add points
    if (!event.capturerIsWhite && !event.capturedPiece.empty()) {
        int points = event.scoreValue;
        score_ += points;
        
        std::cout << "Black: Black captured " << event.capturedPiece 
                  << " at " << event.position 
                  << " (++" << points << " points, total: " << score_ << ")" << std::endl;
    }
}

int BlackScoreTracker::getScore() const {
    return score_;
}

void BlackScoreTracker::resetScore() {
    score_ = 0;
}

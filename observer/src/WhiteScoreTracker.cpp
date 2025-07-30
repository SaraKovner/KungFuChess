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

void WhiteScoreTracker::onNotify(const BaseEvent& event) {
    // Check if this is a capture event
    if (auto* captureEvent = dynamic_cast<const CaptureEvent*>(&event)) {
        handleCapture(*captureEvent);
    }
    // Other events are ignored
}

void WhiteScoreTracker::handleCapture(const CaptureEvent& event) {
    // If white captured black piece - add points
    if (event.capturerIsWhite && !event.capturedPiece.empty()) {
        int points = event.scoreValue;
        score_ += points;
        
        std::cout << "Trophy: White captured " << event.capturedPiece 
                  << " at " << event.position 
                  << " (++" << points << " points, total: " << score_ << ")" << std::endl;
    }
}

int WhiteScoreTracker::getScore() const {
    return score_;
}

void WhiteScoreTracker::resetScore() {
    score_ = 0;
}

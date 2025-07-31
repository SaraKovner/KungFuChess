#pragma once
#include <string>
#include "BaseEvent.hpp"

/**
 * Piece capture event in the game
 */
struct CaptureEvent : public BaseEvent {
    std::string capturingPiece;  // The piece that captures
    std::string capturedPiece;   // The piece that is captured
    std::string position;        // Capture position
    bool capturerIsWhite;       // Is the capturing piece white
    double timestamp;           // Capture time
    int scoreValue;             // Score value of captured piece
    
    CaptureEvent(const std::string& capturing, const std::string& captured, 
                const std::string& pos, bool capturerWhite, int score = 0, double time = 0.0)
        : capturingPiece(capturing), capturedPiece(captured), position(pos), 
          capturerIsWhite(capturerWhite), scoreValue(score), timestamp(time) {}
};

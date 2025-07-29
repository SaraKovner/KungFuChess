#pragma once
#include <string>

/**
 * Piece move event in the game
 */
struct MoveEvent {
    std::string piece;        // Piece type (P=pawn, R=rook, etc.)
    std::string from;         // Source position (e.g. "e2")
    std::string to;           // Target position (e.g. "e4")
    bool isWhite;            // Is this a white piece
    double timestamp;        // Move time
    
    MoveEvent(const std::string& p, const std::string& f, const std::string& t, bool white, double time = 0.0)
        : piece(p), from(f), to(t), isWhite(white), timestamp(time) {}
};

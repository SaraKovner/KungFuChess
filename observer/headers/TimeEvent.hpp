#pragma once

/**
 * Time update event in the game
 */
struct TimeEvent {
    double whiteTimeLeft;       // Time left for white (in seconds)
    double blackTimeLeft;       // Time left for black (in seconds)
    double gameTime;           // Total game time
    bool isWhiteTurn;          // Is it white's turn now
    
    TimeEvent(double whiteTime, double blackTime, double totalTime, bool whiteTurn)
        : whiteTimeLeft(whiteTime), blackTimeLeft(blackTime), 
          gameTime(totalTime), isWhiteTurn(whiteTurn) {}
};

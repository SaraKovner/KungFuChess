#pragma once
#include <string>

/**
 * Game state change event
 */
enum class GameState {
    Playing,
    WhiteWin,
    BlackWin,
    Draw,
    Paused
};

struct GameStateEvent {
    GameState newState;         // New state
    GameState previousState;    // Previous state
    std::string reason;         // Reason for change
    double timestamp;          // Time of change
    
    GameStateEvent(GameState newSt, GameState prevSt, const std::string& r = "", double time = 0.0)
        : newState(newSt), previousState(prevSt), reason(r), timestamp(time) {}
};

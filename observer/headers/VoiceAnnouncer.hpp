#pragma once
#include "Observer.hpp"
#include "MoveEvent.hpp"
#include "CaptureEvent.hpp"
#include "GameStateEvent.hpp"
#include <string>

/**
 * Voice and visual announcements system for the game
 * Listens to moves, captures and state changes
 */
class VoiceAnnouncer : public Observer<MoveEvent>, 
                      public Observer<CaptureEvent>, 
                      public Observer<GameStateEvent> {
private:
    std::string getPieceNameEnglish(const std::string& piece) const;
    std::string getPlayerName(bool isWhite) const;
    
public:
    // Listen to moves
    void onNotify(const MoveEvent& event) override;
    
    // Listen to captures
    void onNotify(const CaptureEvent& event) override;
    
    // Listen to state changes
    void onNotify(const GameStateEvent& event) override;
};

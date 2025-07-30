#pragma once
#include "Observer.hpp"
#include "MoveEvent.hpp"
#include "CaptureEvent.hpp"
#include "GameStateEvent.hpp"
#include "BaseEvent.hpp"
#include <string>

/**
 * Voice and visual announcements system for the game
 * Listens to moves, captures and state changes
 */
class VoiceAnnouncer : public Observer {
private:
    std::string getPieceNameEnglish(const std::string& piece) const;
    std::string getPlayerName(bool isWhite) const;
    
    void announceMove(const MoveEvent& event);
    void announceCapture(const CaptureEvent& event);
    void announceGameState(const GameStateEvent& event);
    
public:
    void onNotify(const BaseEvent& event) override;
};

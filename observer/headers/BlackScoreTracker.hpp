#pragma once
#include "Observer.hpp"
#include "CaptureEvent.hpp"
#include "BaseEvent.hpp"

/**
 * Score tracker for black player
 * Listens to capture events and updates score accordingly
 */
class BlackScoreTracker : public Observer {
private:
    int score_;
    
    // Point values of pieces
    int getPieceValue(const std::string& piece) const;
    void handleCapture(const CaptureEvent& event);
    
public:
    BlackScoreTracker();
    void onNotify(const BaseEvent& event) override;
    
    int getScore() const;
    void resetScore();
};

#pragma once
#include "Observer.hpp"
#include "CaptureEvent.hpp"

/**
 * Score tracker for black player
 * Listens to capture events and updates score accordingly
 */
class BlackScoreTracker : public Observer<CaptureEvent> {
private:
    int score_;
    
    // Point values of pieces
    int getPieceValue(const std::string& piece) const;
    
public:
    BlackScoreTracker();
    void onNotify(const CaptureEvent& event) override;
    
    int getScore() const;
    void resetScore();
};

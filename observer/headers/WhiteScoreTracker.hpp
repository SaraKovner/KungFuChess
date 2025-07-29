#pragma once
#include "Observer.hpp"
#include "CaptureEvent.hpp"

/**
 * Score tracker for white player
 * Listens to capture events and updates score accordingly
 */
class WhiteScoreTracker : public Observer<CaptureEvent> {
private:
    int score_;
    
    // Point values of pieces
    int getPieceValue(const std::string& piece) const;
    
public:
    WhiteScoreTracker();
    void onNotify(const CaptureEvent& event) override;
    
    int getScore() const;
    void resetScore();
};

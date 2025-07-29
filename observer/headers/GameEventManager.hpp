#pragma once
#include "Subject.hpp"
#include "MoveEvent.hpp"
#include "CaptureEvent.hpp"
#include "GameStateEvent.hpp"
#include "TimeEvent.hpp"

/**
 * Central manager for all game events
 * Combines professional PubSub approach with simple EventDispatcher
 */
class GameEventManager {
private:
    // Separate subjects for each event type - type safe and professional
    Subject<MoveEvent> moveSubject_;
    Subject<CaptureEvent> captureSubject_;
    Subject<GameStateEvent> stateSubject_;
    Subject<TimeEvent> timeSubject_;
    
public:
    // === Move subscription ===
    void subscribeToMoves(Observer<MoveEvent>* observer);
    void unsubscribeFromMoves(Observer<MoveEvent>* observer);
    void publishMove(const MoveEvent& event);
    
    // === Capture subscription ===
    void subscribeToCaptures(Observer<CaptureEvent>* observer);
    void unsubscribeFromCaptures(Observer<CaptureEvent>* observer);
    void publishCapture(const CaptureEvent& event);
    
    // === Game state subscription ===
    void subscribeToGameState(Observer<GameStateEvent>* observer);
    void unsubscribeFromGameState(Observer<GameStateEvent>* observer);
    void publishGameState(const GameStateEvent& event);
    
    // === Time subscription ===
    void subscribeToTime(Observer<TimeEvent>* observer);
    void unsubscribeFromTime(Observer<TimeEvent>* observer);
    void publishTime(const TimeEvent& event);
    
    // === Statistics ===
    size_t getMoveObserverCount() const;
    size_t getCaptureObserverCount() const;
    size_t getStateObserverCount() const;
    size_t getTimeObserverCount() const;
};

#include "../headers/GameEventManager.hpp"

// === Move subscription ===
void GameEventManager::subscribeToMoves(Observer<MoveEvent>* observer) {
    moveSubject_.subscribe(observer);
}

void GameEventManager::unsubscribeFromMoves(Observer<MoveEvent>* observer) {
    moveSubject_.unsubscribe(observer);
}

void GameEventManager::publishMove(const MoveEvent& event) {
    moveSubject_.notify(event);
}

// === Capture subscription ===
void GameEventManager::subscribeToCaptures(Observer<CaptureEvent>* observer) {
    captureSubject_.subscribe(observer);
}

void GameEventManager::unsubscribeFromCaptures(Observer<CaptureEvent>* observer) {
    captureSubject_.unsubscribe(observer);
}

void GameEventManager::publishCapture(const CaptureEvent& event) {
    captureSubject_.notify(event);
}

// === Game state subscription ===
void GameEventManager::subscribeToGameState(Observer<GameStateEvent>* observer) {
    stateSubject_.subscribe(observer);
}

void GameEventManager::unsubscribeFromGameState(Observer<GameStateEvent>* observer) {
    stateSubject_.unsubscribe(observer);
}

void GameEventManager::publishGameState(const GameStateEvent& event) {
    stateSubject_.notify(event);
}

// === Time subscription ===
void GameEventManager::subscribeToTime(Observer<TimeEvent>* observer) {
    timeSubject_.subscribe(observer);
}

void GameEventManager::unsubscribeFromTime(Observer<TimeEvent>* observer) {
    timeSubject_.unsubscribe(observer);
}

void GameEventManager::publishTime(const TimeEvent& event) {
    timeSubject_.notify(event);
}

// === Statistics ===
size_t GameEventManager::getMoveObserverCount() const {
    return moveSubject_.getObserverCount();
}

size_t GameEventManager::getCaptureObserverCount() const {
    return captureSubject_.getObserverCount();
}

size_t GameEventManager::getStateObserverCount() const {
    return stateSubject_.getObserverCount();
}

size_t GameEventManager::getTimeObserverCount() const {
    return timeSubject_.getObserverCount();
}

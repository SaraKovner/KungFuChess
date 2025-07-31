#pragma once
#include <vector>
#include <algorithm>
#include "Observer.hpp"
#include "BaseEvent.hpp"

/**
 * Simple Subject - manages list of observers for events
 */
class Subject {
private:
    std::vector<Observer*> observers_;

public:
    virtual ~Subject() = default;

    /**
     * Add new observer
     */
    void subscribe(Observer* observer) {
        if (observer && std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
            observers_.push_back(observer);
        }
    }

    /**
     * Remove observer
     */
    void unsubscribe(Observer* observer) {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
    }

    /**
     * Notify all observers
     */
    void notify(const BaseEvent& event) {
        for (auto* observer : observers_) {
            if (observer) {
                observer->onNotify(event);
            }
        }
    }

    /**
     * Number of registered observers
     */
    size_t getObserverCount() const {
        return observers_.size();
    }
};

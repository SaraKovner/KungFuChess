#pragma once
#include <vector>
#include <algorithm>
#include "Observer.hpp"

/**
 * Advanced Subject with template - manages list of listeners for specific event type
 * T = event type
 */
template <typename T>
class Subject {
private:
    std::vector<Observer<T>*> observers_;

public:
    virtual ~Subject() = default;

    /**
     * Add new listener
     */
    void subscribe(Observer<T>* observer) {
        if (observer && std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
            observers_.push_back(observer);
        }
    }

    /**
     * Remove listener
     */
    void unsubscribe(Observer<T>* observer) {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
    }

    /**
     * Send message to all listeners
     */
    void notify(const T& event) {
        for (auto* observer : observers_) {
            if (observer) {
                observer->onNotify(event);
            }
        }
    }

    /**
     * Number of registered listeners
     */
    size_t getObserverCount() const {
        return observers_.size();
    }
};

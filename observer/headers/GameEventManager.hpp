#pragma once
#include <map>
#include <string>
#include "Subject.hpp"
#include "BaseEvent.hpp"

/**
 * Flexible GameEventManager with Map - maximum flexibility
 */
class GameEventManager {
private:
    std::map<std::string, Subject> subjects_;
    
public:
    /**
     * Subscribe observer to specific event type
     */
    void subscribe(Observer* observer, const std::string& eventType) {
        subjects_[eventType].subscribe(observer);
    }
    
    /**
     * Unsubscribe observer from specific event type
     */
    void unsubscribe(Observer* observer, const std::string& eventType) {
        if (subjects_.find(eventType) != subjects_.end()) {
            subjects_[eventType].unsubscribe(observer);
        }
    }
    
    /**
     * Publish event to all subscribers of that event type
     */
    void publish(const BaseEvent& event, const std::string& eventType) {
        if (subjects_.find(eventType) != subjects_.end()) {
            subjects_[eventType].notify(event);
        }
    }
    
    /**
     * Get number of observers for specific event type
     */
    size_t getObserverCount(const std::string& eventType) const {
        auto it = subjects_.find(eventType);
        return (it != subjects_.end()) ? it->second.getObserverCount() : 0;
    }
};

#pragma once
#include "BaseEvent.hpp"

/**
 * Simple Observer interface - receives BaseEvent and handles type checking internally
 */
class Observer {
public:
    virtual ~Observer() = default;
    
    /**
     * Called when an event occurs
     * @param event The event that occurred
     */
    virtual void onNotify(const BaseEvent& event) = 0;
};

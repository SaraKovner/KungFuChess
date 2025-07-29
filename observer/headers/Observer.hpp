#pragma once

/**
 * Base Observer interface with template - professional and flexible
 * T = type of event that the listener receives
 */
template <typename T>
class Observer {
public:
    virtual ~Observer() = default;
    
    /**
     * Called when an event occurs
     * @param event The event that occurred
     */
    virtual void onNotify(const T& event) = 0;
};

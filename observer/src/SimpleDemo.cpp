#include "../headers/GameEventManager.hpp"
#include "../headers/WhiteScoreTracker.hpp"
#include "../headers/VoiceAnnouncer.hpp"
#include "../headers/CaptureEvent.hpp"
#include "../headers/MoveEvent.hpp"
#include <iostream>

/**
 * Simple demonstration of the new flexible Observer architecture
 */
int main() {
    std::cout << "Simple Observer Demo - Flexible Architecture" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Create event manager
    GameEventManager eventManager;
    
    // Create observers
    WhiteScoreTracker scoreTracker;
    VoiceAnnouncer announcer;
    
    // Easy registration to multiple event types
    eventManager.subscribe(&announcer, "move");
    eventManager.subscribe(&announcer, "capture");
    eventManager.subscribe(&scoreTracker, "capture");
    
    std::cout << "\nObservers registered:" << std::endl;
    std::cout << "- Move events: " << eventManager.getObserverCount("move") << " observers" << std::endl;
    std::cout << "- Capture events: " << eventManager.getObserverCount("capture") << " observers" << std::endl;
    
    // Publish events
    std::cout << "\nPublishing events..." << std::endl;
    
    MoveEvent move("P", "e2", "e4", true);
    eventManager.publish(move, "move");
    
    CaptureEvent capture("Q", "P", "e4", true, 1);
    eventManager.publish(capture, "capture");
    
    // Easy to add new event types
    eventManager.subscribe(&announcer, "powerup");
    std::cout << "\nAdded new event type 'powerup' - " 
              << eventManager.getObserverCount("powerup") << " observers" << std::endl;
    
    std::cout << "\nFinal score: " << scoreTracker.getScore() << std::endl;
    std::cout << "Demo completed!" << std::endl;
    
    return 0;
}
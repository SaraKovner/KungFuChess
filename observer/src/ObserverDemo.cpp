#include "../headers/GameEventManager.hpp"
#include "../headers/WhiteScoreTracker.hpp"
#include "../headers/VoiceAnnouncer.hpp"
#include "../headers/MoveEvent.hpp"
#include "../headers/CaptureEvent.hpp"
#include "../headers/GameStateEvent.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

/**
 * Demo of flexible Observer system with Map-based architecture
 */
class ObserverDemo {
private:
    GameEventManager eventManager_;
    
    // Different observers
    WhiteScoreTracker whiteScore_;
    VoiceAnnouncer announcer_;
    
public:
    void setup() {
        std::cout << "Setting up flexible Observer system..." << std::endl;
        
        // Register VoiceAnnouncer to multiple event types
        eventManager_.subscribe(&announcer_, "move");
        eventManager_.subscribe(&announcer_, "capture");
        eventManager_.subscribe(&announcer_, "gamestate");
        
        // Register WhiteScoreTracker only to captures
        eventManager_.subscribe(&whiteScore_, "capture");
        
        std::cout << "All observers registered successfully!" << std::endl;
        printStatistics();
    }
    
    void runDemo() {
        std::cout << "\nStarting KungFu Chess demo...\n" << std::endl;
        
        // Game simulation
        simulateGame();
        
        // Show summary
        showGameSummary();
    }
    
private:
    void simulateGame() {
        // Game opening
        GameStateEvent gameStart(GameState::Playing, GameState::Paused, "New game started");
        eventManager_.publish(gameStart, "gamestate");
        
        sleep(1);
        
        // Opening moves
        MoveEvent move1("P", "e2", "e4", true, 1.2);
        eventManager_.publish(move1, "move");
        sleep(500);
        
        MoveEvent move2("P", "e7", "e5", false, 2.1);
        eventManager_.publish(move2, "move");
        sleep(500);
        
        MoveEvent move3("N", "g1", "f3", true, 3.0);
        eventManager_.publish(move3, "move");
        sleep(500);
        
        MoveEvent move4("N", "b8", "c6", false, 4.2);
        eventManager_.publish(move4, "move");
        sleep(500);
        
        // First capture!
        CaptureEvent capture1("N", "P", "e5", true, 1, 8.5);
        eventManager_.publish(capture1, "capture");
        sleep(1000);
        
        // Black response
        CaptureEvent capture2("N", "N", "f3", false, 3, 9.8);
        eventManager_.publish(capture2, "capture");
        sleep(1000);
        
        // More moves
        MoveEvent move5("Q", "d1", "h5", true, 12.1);
        eventManager_.publish(move5, "move");
        sleep(500);
        
        MoveEvent move6("Q", "d8", "f6", false, 13.4);
        eventManager_.publish(move6, "move");
        sleep(500);
        
        // Queen capture!
        CaptureEvent capture3("Q", "Q", "f6", true, 9, 16.7);
        eventManager_.publish(capture3, "capture");
        sleep(1000);
        
        // Game end
        GameStateEvent gameEnd(GameState::WhiteWin, GameState::Playing, "Black resigned");
        eventManager_.publish(gameEnd, "gamestate");
    }
    
    void showGameSummary() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "Game Summary" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::cout << "\nFinal Score:" << std::endl;
        std::cout << "   White: " << whiteScore_.getScore() << " points" << std::endl;
    }
    
    void printStatistics() {
        std::cout << "\nObserver Statistics:" << std::endl;
        std::cout << "   Move observers: " << eventManager_.getObserverCount("move") << std::endl;
        std::cout << "   Capture observers: " << eventManager_.getObserverCount("capture") << std::endl;
        std::cout << "   Game state observers: " << eventManager_.getObserverCount("gamestate") << std::endl;
    }
    
    void sleep(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
};

int main() {
    std::cout << "KungFu Chess - Advanced Observer System" << std::endl;
    std::cout << "Combining PubSub professionalism with Interface simplicity" << std::endl;
    std::cout << "=================================" << std::endl;
    
    ObserverDemo demo;
    demo.setup();
    demo.runDemo();
    
    std::cout << "\nDemo completed successfully!" << std::endl;
    return 0;
}

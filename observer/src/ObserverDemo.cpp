#include "../headers/GameEventManager.hpp"
#include "../headers/WhiteScoreTracker.hpp"
#include "../headers/BlackScoreTracker.hpp"
#include "../headers/WhiteMovesTable.hpp"
#include "../headers/BlackMovesTable.hpp"
#include "../headers/VoiceAnnouncer.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

/**
 * Complete demo of integrated Observer system
 * Demonstrates how to combine professionalism with simplicity
 */
class ObserverDemo {
private:
    GameEventManager eventManager_;
    
    // Different listeners
    WhiteScoreTracker whiteScore_;
    BlackScoreTracker blackScore_;
    WhiteMovesTable whiteMoves_;
    BlackMovesTable blackMoves_;
    VoiceAnnouncer announcer_;
    
public:
    void setup() {
        std::cout << "Setting up advanced Observer system..." << std::endl;
        
        // Subscribe to moves
        eventManager_.subscribeToMoves(&whiteMoves_);
        eventManager_.subscribeToMoves(&blackMoves_);
        eventManager_.subscribeToMoves(&announcer_);
        
        // Subscribe to captures
        eventManager_.subscribeToCaptures(&whiteScore_);
        eventManager_.subscribeToCaptures(&blackScore_);
        eventManager_.subscribeToCaptures(&announcer_);
        
        // Subscribe to game state
        eventManager_.subscribeToGameState(&announcer_);
        
        std::cout << "All listeners registered successfully!" << std::endl;
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
        eventManager_.publishGameState(GameStateEvent(GameState::Playing, GameState::Paused, "New game started"));
        
        sleep(1);
        
        // Opening moves
        eventManager_.publishMove(MoveEvent("P", "e2", "e4", true, 1.2));
        sleep(500);
        
        eventManager_.publishMove(MoveEvent("P", "e7", "e5", false, 2.1));
        sleep(500);
        
        eventManager_.publishMove(MoveEvent("N", "g1", "f3", true, 3.0));
        sleep(500);
        
        eventManager_.publishMove(MoveEvent("N", "b8", "c6", false, 4.2));
        sleep(500);
        
        // First capture!
        eventManager_.publishCapture(CaptureEvent("N", "P", "e5", true, 1, 8.5));
        sleep(1000);
        
        // Black response
        eventManager_.publishCapture(CaptureEvent("N", "N", "f3", false, 3, 9.8));
        sleep(1000);
        
        // More moves
        eventManager_.publishMove(MoveEvent("Q", "d1", "h5", true, 12.1));
        sleep(500);
        
        eventManager_.publishMove(MoveEvent("Q", "d8", "f6", false, 13.4));
        sleep(500);
        
        // Queen capture!
        eventManager_.publishCapture(CaptureEvent("Q", "Q", "f6", true, 9, 16.7));
        sleep(1000);
        
        // Game end
        eventManager_.publishGameState(GameStateEvent(GameState::WhiteWin, GameState::Playing, "Black resigned"));
    }
    
    void showGameSummary() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "Game Summary" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::cout << "\nFinal Score:" << std::endl;
        std::cout << "   White: " << whiteScore_.getScore() << " points" << std::endl;
        std::cout << "   Black: " << blackScore_.getScore() << " points" << std::endl;
        
        std::cout << "\nMove Statistics:" << std::endl;
        std::cout << "   White: " << whiteMoves_.getMoveCount() << " moves" << std::endl;
        std::cout << "   Black: " << blackMoves_.getMoveCount() << " moves" << std::endl;
        
        // Show all moves
        whiteMoves_.printAllMoves();
        blackMoves_.printAllMoves();
    }
    
    void printStatistics() {
        std::cout << "\nListener Statistics:" << std::endl;
        std::cout << "   Move listeners: " << eventManager_.getMoveObserverCount() << std::endl;
        std::cout << "   Capture listeners: " << eventManager_.getCaptureObserverCount() << std::endl;
        std::cout << "   Game state listeners: " << eventManager_.getStateObserverCount() << std::endl;
        std::cout << "   Time listeners: " << eventManager_.getTimeObserverCount() << std::endl;
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

#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "../../shared/network/NetworkInterface.hpp"

// Forward declaration
class Game;

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

enum class PlayerColor { WHITE, BLACK, UNKNOWN };

class GameClient : public NetworkInterface {
private:
    int socket_fd_;
    std::string server_address_;
    int server_port_;
    std::atomic<bool> connected_{false};
    PlayerColor my_color_{PlayerColor::UNKNOWN};
    Game* game_{nullptr}; // Reference to the game instance
    
    // Helper function to get player ID based on color
    int getMyPlayerId() const {
        return (my_color_ == PlayerColor::WHITE) ? 1 : 2;
    }
    
public:
    GameClient(const std::string& server_address, int port = 8080);
    ~GameClient();
    
    bool connect();
    void run();
    void disconnect();
    
    // Set game instance for network synchronization
    void setGame(Game* game);
    
    // NetworkInterface implementation
    void sendMove(const std::string& move) override;
    void onMoveReceived(const std::string& move) override;
    
private:
    void receiveMessages();
    void sendMessage(const std::string& message);
    void handleMessage(const std::string& message);
    void handleServerCommand(const std::string& command);  // טיפול בפקודות שרת
    void simulateGameplay();
    
    void initializeWinsock();
    void cleanupWinsock();
};

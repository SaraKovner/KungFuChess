#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <queue>
#include <memory>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

// Game engine includes
#include "../../shared/core/Game.hpp"
#include "../../shared/core/Board.hpp"
#include "../../shared/game_logic/PieceFactory.hpp"
#include "../../shared/graphics/img/ImgFactory.hpp"
#include "../../shared/graphics/img/MockImg.hpp"

enum class PlayerColor { WHITE, BLACK };

struct PlayerConnection {
    int socket_fd;
    PlayerColor color;
    bool connected;
    std::string name;
};

class GameServer {
private:
    int server_socket_;
    int port_;
    std::atomic<bool> running_{false};
    std::vector<PlayerConnection> players_;
    std::mutex players_mutex_;
    
    // Command queue from all clients
    std::queue<std::string> command_queue_;
    std::mutex queue_mutex_;
    
    // Game engine - server is authoritative
    std::unique_ptr<Game> game_;
    std::mutex game_mutex_;

public:
    GameServer(int port = 8080);
    ~GameServer();
    
    bool start();
    void run();
    void stop();
    
private:
    void acceptClients();
    void handleClient(int client_socket, PlayerColor color);
    void broadcastMessage(const std::string& message);
    void processCommands();
    void handleInputCommand(const std::string& command);  // טיפול בקלט מקליינטים - שלח פקודות חזרה
    
    // Game logic functions
    void initializeGame();
    bool validateMove(int player_id, const std::string& move);
    void applyMove(int player_id, const std::string& move);
    void broadcastGameState();
    void checkWinCondition();
    
    // Utility functions
    void sendToClient(int socket, const std::string& message);
    std::string receiveFromClient(int socket);
    void initializeWinsock();
    void cleanupWinsock();
};

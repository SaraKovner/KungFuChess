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
#include "../game_logic/ServerGameEngine.hpp"

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
    std::unique_ptr<ServerGameEngine> server_engine_;
    std::mutex game_mutex_;
    
    // Server-side player state tracking
    std::mutex player_state_mutex_;

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
    bool validateInput(int player_id, const std::string& cmd_type);
    void processValidatedInput(int player_id, const std::string& cmd_type);
    void broadcastGameState();
    void checkWinCondition();
    
    // Server-side game state validation
    bool isValidCursorMove(int player_id, const std::string& direction);
    bool isValidPieceSelection(int player_id);
    bool isValidPieceMove(int player_id);
    bool validateSelectCommand(int player_id);
    
    // Server-side player state
    struct PlayerState {
        std::pair<int, int> cursor_pos = {-1, -1};
        std::pair<int, int> selected_piece_pos = {-1, -1};
        bool has_selected_piece = false;
    };
    PlayerState player1_state_;
    PlayerState player2_state_;
    
    // Utility functions
    void sendToClient(int socket, const std::string& message);
    std::string receiveFromClient(int socket);
    void initializeWinsock();
    void cleanupWinsock();
};

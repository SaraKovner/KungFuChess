#include "GameClient.hpp"
#include "../core/Game.hpp"
#include "../graphics/img/OpenCvImg.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <windows.h>

GameClient::GameClient(const std::string& server_address, int port) 
    : server_address_(server_address), server_port_(port), socket_fd_(-1) {
    initializeWinsock();
}

GameClient::~GameClient() {
    disconnect();
    cleanupWinsock();
}

void GameClient::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }
#endif
}

void GameClient::cleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

bool GameClient::connect() {
    // Create socket
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }

    // Connect to server
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);
    
    if (inet_pton(AF_INET, server_address_.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << server_address_ << std::endl;
        return false;
    }

    if (::connect(socket_fd_, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed to " << server_address_ << ":" << server_port_ << std::endl;
        return false;
    }

    connected_ = true;
    std::cout << "✅ Connected to server " << server_address_ << ":" << server_port_ << std::endl;
    return true;
}

void GameClient::run() {
    if (!connect()) {
        return;
    }

    // Start receiving messages in separate thread
    std::thread receive_thread(&GameClient::receiveMessages, this);
    
    // Simulate some gameplay
    simulateGameplay();
    
    receive_thread.join();
}

void GameClient::receiveMessages() {
    char buffer[1024];
    std::string accumulated_data;
    
    while (connected_) {
        int bytes_received = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "🔌 Disconnected from server" << std::endl;
            connected_ = false;
            break;
        }
        
        buffer[bytes_received] = '\0';
        accumulated_data += std::string(buffer);
        
        // Process all complete messages (separated by newlines)
        size_t pos = 0;
        while ((pos = accumulated_data.find('\n')) != std::string::npos) {
            std::string message = accumulated_data.substr(0, pos);
            accumulated_data.erase(0, pos + 1);
            
            if (!message.empty()) {
                handleMessage(message);
            }
        }
    }
}

void GameClient::handleMessage(const std::string& message) {
    std::cout << "📨 Received: " << message << std::endl;
    
    if (message.find("COLOR:") == 0) {
        std::string color = message.substr(6);
        if (color == "WHITE") {
            my_color_ = PlayerColor::WHITE;
            std::cout << "🤍 You are playing as WHITE" << std::endl;
        } else if (color == "BLACK") {
            my_color_ = PlayerColor::BLACK;
            std::cout << "🖤 You are playing as BLACK" << std::endl;
        }
        std::cout << "🎯 Color set to: " << (my_color_ == PlayerColor::WHITE ? "WHITE" : "BLACK") << std::endl;
    } else if (message == "GAME_START") {
        std::cout << "🎮 Game started! Both players connected." << std::endl;
    } else if (message.find("MOVE:") == 0) {
        std::string move = message.substr(5);
        onMoveReceived(move);
    } else if (message.find("COMMAND:") == 0) {
        std::string command = message.substr(8);
        std::cout << "🎯 Game command: " << command << std::endl;
    } else if (message.find("SERVER_CMD:") == 0) {
        std::string command = message.substr(11); // Remove "SERVER_CMD:"
        handleServerCommand(command);
    }
}

void GameClient::simulateGameplay() {
    std::cout << "🚀 simulateGameplay() started" << std::endl;
    
    // Wait for color assignment
    std::cout << "⏳ Waiting for color assignment..." << std::endl;
    while (my_color_ == PlayerColor::UNKNOWN && connected_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    if (!connected_) {
        std::cout << "❌ Connection lost during color wait" << std::endl;
        return;
    }
    
    std::cout << "✅ Color assigned: " << (my_color_ == PlayerColor::WHITE ? "WHITE" : "BLACK") << std::endl;
    
    // Wait for game start
    std::cout << "⏳ Waiting for game start..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    try {
        // Create the actual KungFu Chess game
        std::cout << "🔧 Creating image factory..." << std::endl;
        
        // Create unique window name for each client
        std::string window_name = "KungFu Chess - " + std::string(my_color_ == PlayerColor::WHITE ? "WHITE" : "BLACK") + " Player";
        std::cout << "🪟 Creating window: " << window_name << std::endl;
        
        auto img_factory = std::make_shared<OpenCvImgFactory>(window_name);
        
        // Get current working directory and construct absolute path
        char current_dir[1024];
        if (GetCurrentDirectory(sizeof(current_dir), (LPTSTR)current_dir)) {
            std::string pieces_root = std::string(current_dir) + "\\assets\\pieces\\";
            std::cout << "🗂️ Using assets path: " << pieces_root << std::endl;
            
            std::cout << "🎮 Starting KungFu Chess game..." << std::endl;
            std::cout << "Color: " << (my_color_ == PlayerColor::WHITE ? "WHITE" : "BLACK") << std::endl;
            
            std::cout << "🔧 Creating game instance..." << std::endl;
            
            // Add synchronization before creating OpenCV windows
            std::cout << "🖼️ Initializing graphics for " << (my_color_ == PlayerColor::WHITE ? "WHITE" : "BLACK") << " player..." << std::endl;
            
            auto game = create_game(pieces_root, img_factory);
            
            std::cout << "🎯 Event listeners initializing..." << std::endl;
            // Connect network interface to game
            game.setNetworkInterface(this);
            
            // Set which player this client represents
            int my_player_id = getMyPlayerId();
            game.setMyPlayerId(my_player_id);
            
            // Connect game instance to client for receiving moves
            setGame(&game);
            
            std::cout << "🔗 Connected game to network interface" << std::endl;
            
            std::cout << "🚀 Running game.run()..." << std::endl;
            // Run the game in display-only mode - this will open the game window!
            // TODO: Implement display-only mode that doesn't process input locally
            game.run(-1, true);
            
            std::cout << "🖼️ GRAPHICS WINDOW OPENED FOR: " << (my_color_ == PlayerColor::WHITE ? "WHITE" : "BLACK") << " PLAYER!" << std::endl;
            std::cout << "✅ Game.run() completed!" << std::endl;
        } else {
            std::cerr << "❌ Failed to get current directory" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Game error: " << e.what() << std::endl;
    }
    
    std::cout << "🏁 Game finished. Press Enter to exit..." << std::endl;
    std::cin.get();
}

void GameClient::sendMessage(const std::string& message) {
    if (!connected_) return;
    
    std::string msg = message + "\n";
    send(socket_fd_, msg.c_str(), msg.length(), 0);
}

void GameClient::disconnect() {
    connected_ = false;
    if (socket_fd_ >= 0) {
#ifdef _WIN32
        closesocket(socket_fd_);
#else
        close(socket_fd_);
#endif
        socket_fd_ = -1;
    }
}

// NetworkInterface implementation
void GameClient::sendMove(const std::string& move) {
    if (connected_) {
        std::cout << "🌐 Sending to server: " << move << std::endl;
        std::cout << "🔍 My color when sending: " << (my_color_ == PlayerColor::WHITE ? "WHITE" : 
                                                    my_color_ == PlayerColor::BLACK ? "BLACK" : "UNKNOWN") << std::endl;
        
        // If this is an input command, send it directly without MOVE: prefix
        if (move.find("INPUT:") == 0) {
            sendMessage(move);
        } else {
            sendMessage("MOVE:" + move);
        }
        std::cout << "🌐 Sent move to server: " << move << std::endl;
    }
}

void GameClient::onMoveReceived(const std::string& move) {
    std::cout << "🌐 Received move from server: " << move << std::endl;
    
    // Apply move to local game if we have a game instance
    if (game_) {
        game_->applyNetworkMove(move);
    } else {
        std::cerr << "❌ No game instance to apply move to" << std::endl;
    }
}

void GameClient::setGame(Game* game) {
    game_ = game;
    std::cout << "🔗 Game instance connected to network client" << std::endl;
}

void GameClient::handleServerCommand(const std::string& command) {
    std::cout << "🎯 Processing server command: " << command << std::endl;
    std::cout << "🔍 My color: " << (my_color_ == PlayerColor::WHITE ? "WHITE" : 
                                  my_color_ == PlayerColor::BLACK ? "BLACK" : "UNKNOWN") << std::endl;
    
    // Parse player_id:cmd_type
    size_t colon_pos = command.find(':');
    if (colon_pos != std::string::npos) {
        std::string player_id_str = command.substr(0, colon_pos);
        std::string cmd_type = command.substr(colon_pos + 1);
        
        int player_id = std::stoi(player_id_str);
        int my_player_id = getMyPlayerId();
        
        std::cout << "🔍 Command for player: " << player_id << ", I am player: " << my_player_id << std::endl;
        
        // Process all commands from server to ensure synchronization
        // The server is the authoritative source for all game state
        std::cout << "🎯 Applying server command: Player " << player_id << " -> " << cmd_type << std::endl;
        
        // Apply command to local game if we have a game instance
        if (game_) {
            game_->processServerInput(player_id, cmd_type);
            std::cout << "✅ Server command applied to local game" << std::endl;
        } else {
            std::cerr << "❌ No game instance to apply server command to" << std::endl;
        }
    } else {
        std::cerr << "❌ Invalid server command format: " << command << std::endl;
    }
}
#include "GameServer.hpp"
#include <iostream>
#include <thread>

GameServer::GameServer(int port) : port_(port), server_socket_(-1) {
    initializeWinsock();
}

GameServer::~GameServer() {
    stop();
    cleanupWinsock();
}

void GameServer::initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }
#endif
}

void GameServer::cleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

bool GameServer::start() {
    // Create socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }

    // Bind socket
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return false;
    }

    // Listen
    if (listen(server_socket_, 2) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        return false;
    }

    running_ = true;
    std::cout << "🎮 KungFu Chess Server started on port " << port_ << std::endl;
    std::cout << "Waiting for 2 players to connect..." << std::endl;
    
    return true;
}

void GameServer::run() {
    if (!start()) {
        return;
    }

    // Start accepting clients in separate thread
    std::thread accept_thread(&GameServer::acceptClients, this);
    
    // Start command processing
    std::thread command_thread(&GameServer::processCommands, this);
    
    accept_thread.join();
    command_thread.join();
}

void GameServer::acceptClients() {
    while (running_ && players_.size() < 2) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            if (running_) {
                std::cerr << "Error accepting client" << std::endl;
            }
            continue;
        }

        // Assign color based on connection order - FIXED
        PlayerColor color = (players_.size() == 0) ? PlayerColor::WHITE : PlayerColor::BLACK;
        std::string color_name = (color == PlayerColor::WHITE) ? "WHITE" : "BLACK";
        
        {
            std::lock_guard<std::mutex> lock(players_mutex_);
            players_.push_back({client_socket, color, true, color_name});
        }
        
        std::cout << "Player " << (players_.size()) << " connected as " << color_name << std::endl;
        
        // Send color assignment to client
        sendToClient(client_socket, "COLOR:" + color_name);
        
        // Start handling this client
        std::thread client_thread(&GameServer::handleClient, this, client_socket, color);
        client_thread.detach();
        
        if (players_.size() == 2) {
            std::cout << "Both players connected! Initializing game..." << std::endl;
            initializeGame();
            broadcastMessage("GAME_START");
            broadcastGameState();
        }
    }
}

void GameServer::handleClient(int client_socket, PlayerColor color) {
    while (running_) {
        std::string message = receiveFromClient(client_socket);
        if (message.empty()) {
            std::cout << "📤 Client " << ((color == PlayerColor::WHITE) ? "WHITE" : "BLACK") << " disconnected" << std::endl;
            break; // Client disconnected
        }
        
        std::cout << "Received from " << ((color == PlayerColor::WHITE) ? "WHITE" : "BLACK") 
                  << ": " << message << std::endl;
        
        // Handle different message types
        if (message.find("MOVE:") == 0) {
            // This is a move command - broadcast to all clients
            std::cout << "🎯 Broadcasting move: " << message << std::endl;
            broadcastMessage(message);
        } else if (message.find("INPUT:") == 0) {
            // This is player input - process through server game
            std::cout << "🎯 Received input from " << ((color == PlayerColor::WHITE) ? "WHITE" : "BLACK") 
                      << ": " << message << std::endl;
            handleInputCommand(message);
        } else {
            // Add other commands to queue for processing
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                command_queue_.push(message);
            }
            
            // For now, just broadcast the command to all clients
            broadcastMessage("COMMAND:" + message);
        }
    }
    
    std::cout << "📤 Client " << ((color == PlayerColor::WHITE) ? "WHITE" : "BLACK") << " handler thread ended" << std::endl;
}

void GameServer::broadcastMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(players_mutex_);
    for (auto& player : players_) {
        if (player.connected) {
            sendToClient(player.socket_fd, message);
        }
    }
}

void GameServer::processCommands() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while (!command_queue_.empty()) {
            std::string command = command_queue_.front();
            command_queue_.pop();
            
            // For now, just log the command
            std::cout << "Processing command: " << command << std::endl;
        }
    }
}

void GameServer::sendToClient(int socket, const std::string& message) {
    std::string msg = message + "\n";
    int result = send(socket, msg.c_str(), msg.length(), 0);
    if (result <= 0) {
        std::cout << "📤 Failed to send message to client: " << message << std::endl;
    }
}

std::string GameServer::receiveFromClient(int socket) {
    char buffer[1024] = {0};
    int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            std::cout << "📤 Client closed connection gracefully" << std::endl;
        } else {
            std::cout << "📤 Client connection error: " << bytes_received << std::endl;
        }
        return "";
    }
    
    std::string message(buffer, bytes_received);
    // Remove newline if present
    if (!message.empty() && message.back() == '\n') {
        message.pop_back();
    }
    return message;
}

void GameServer::stop() {
    running_ = false;
    if (server_socket_ >= 0) {
#ifdef _WIN32
        closesocket(server_socket_);
#else
        close(server_socket_);
#endif
    }
}

void GameServer::handleInputCommand(const std::string& command) {
    std::cout << "🎯 Server processing input: " << command << std::endl;
    
    // Parse INPUT:player_id:cmd_type
    if (command.find("INPUT:") == 0) {
        std::string input_data = command.substr(6); // Remove "INPUT:"
        
        size_t first_colon = input_data.find(':');
        if (first_colon != std::string::npos) {
            std::string player_id_str = input_data.substr(0, first_colon);
            std::string cmd_type = input_data.substr(first_colon + 1);
            
            int player_id = std::stoi(player_id_str);
            
            std::cout << "🎯 Input: Player " << player_id << " -> " << cmd_type << std::endl;
            
            // SERVER VALIDATES INPUT FIRST
            if (validateInput(player_id, cmd_type)) {
                std::cout << "✅ Input validated - processing" << std::endl;
                
                // Process through server game engine
                {
                    std::lock_guard<std::mutex> lock(game_mutex_);
                    if (server_engine_) {
                        std::cout << "🎮 Processing input in server game engine..." << std::endl;
                        
                        if (cmd_type == "up" || cmd_type == "down" || cmd_type == "left" || cmd_type == "right") {
                            server_engine_->updatePlayerCursor(player_id, cmd_type);
                        } else if (cmd_type == "select") {
                            server_engine_->processSelect(player_id);
                        }
                        
                        std::cout << "✅ Server game processing completed" << std::endl;
                    } else {
                        std::cout << "❌ No server engine available" << std::endl;
                    }
                }
                
                // Check win condition AFTER releasing the lock
                std::cout << "🏆 Checking win condition..." << std::endl;
                checkWinCondition();
                
                // ALWAYS BROADCAST - even if no visible change occurred
                std::cout << "📡 About to broadcast to clients..." << std::endl;
                std::string server_command = "SERVER_CMD:" + std::to_string(player_id) + ":" + cmd_type;
                broadcastMessage(server_command);
                std::cout << "📡 Broadcast completed" << std::endl;
                
                std::cout << "✅ Valid input processed and broadcasted" << std::endl;
            } else {
                std::cout << "❌ Input rejected - invalid move" << std::endl;
                // Still broadcast rejection so client knows server received it
                std::string reject_command = "SERVER_CMD:" + std::to_string(player_id) + ":rejected";
                broadcastMessage(reject_command);
                std::cout << "📡 Rejection broadcasted" << std::endl;
            }
        } else {
            std::cout << "❌ Invalid input format: " << command << std::endl;
        }
    } else {
        std::cout << "❌ Not an INPUT command: " << command << std::endl;
    }
}
bool GameServer::validateInput(int player_id, const std::string& cmd_type) {
    std::lock_guard<std::mutex> lock(game_mutex_);
    if (!server_engine_) return false;
    
    // Basic validation - player exists and game is running
    if (player_id < 1 || player_id > 2) return false;
    
    // Movement commands are always valid (cursor movement)
    if (cmd_type == "up" || cmd_type == "down" || cmd_type == "left" || cmd_type == "right") {
        return true; // Always allow cursor movement
    }
    
    // Selection commands - basic validation
    if (cmd_type == "select") {
        return (player_id == 1 || player_id == 2); // Basic player validation
    }
    
    // Exit is always valid
    if (cmd_type == "exit") {
        return true;
    }
    
    return false;
}
void GameServer::initializeGame() {
    std::cout << "🎮 Initializing server game engine..." << std::endl;
    
    try {
        server_engine_ = std::make_unique<ServerGameEngine>();
        server_engine_->initializeStandardGame();
        
        std::cout << "✅ Server game engine initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to initialize game: " << e.what() << std::endl;
    }
}

void GameServer::broadcastGameState() {
    std::lock_guard<std::mutex> lock(game_mutex_);
    if (server_engine_) {
        // TODO: Serialize game state and broadcast to all clients
        std::cout << "📡 Broadcasting game state to all clients" << std::endl;
    }
}

void GameServer::checkWinCondition() {
    std::lock_guard<std::mutex> lock(game_mutex_);
    if (server_engine_) {
        // TODO: Check if game is won and broadcast result
        std::cout << "🏆 Checking win condition..." << std::endl;
    }
}
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
            std::cout << "Both players connected! Game can start!" << std::endl;
            broadcastMessage("GAME_START");
        }
    }
}

void GameServer::handleClient(int client_socket, PlayerColor color) {
    while (running_) {
        std::string message = receiveFromClient(client_socket);
        if (message.empty()) {
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
    
    std::cout << "Client disconnected" << std::endl;
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
    send(socket, msg.c_str(), msg.length(), 0);
}

std::string GameServer::receiveFromClient(int socket) {
    char buffer[1024] = {0};
    int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
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
            
            // For now, just echo the command back to all clients for them to process
            // TODO: Implement server-side game logic here
            std::string server_command = "SERVER_CMD:" + std::to_string(player_id) + ":" + cmd_type;
            broadcastMessage(server_command);
            
            std::cout << "✅ Input broadcasted as server command" << std::endl;
        }
    }
}

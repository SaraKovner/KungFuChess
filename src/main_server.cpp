#include <iostream>
#include <string>
#include "network/GameServer.hpp"

int main(int argc, char* argv[]) {
    int port = 8080; // Default port
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--port=") == 0) {
            port = std::stoi(arg.substr(7));
        }
    }
    
    std::cout << "🎮 KungFu Chess Server starting on port " << port << "..." << std::endl;
    
    GameServer server(port);
    server.run();
    
    return 0;
}

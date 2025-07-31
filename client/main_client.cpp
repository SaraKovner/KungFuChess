#include <iostream>
#include <string>
#include "network/GameClient.hpp"
#include "../shared/core/Game.hpp"
#include "../shared/graphics/img/OpenCvImg.hpp"

int main(int argc, char* argv[]) {
    std::string server_address = "127.0.0.1"; // localhost
    int port = 8080;
    
    bool local_mode = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--local") {
            local_mode = true;
        } else if (arg.find("--server=") == 0) {
            std::string server_info = arg.substr(9);
            size_t colon_pos = server_info.find(':');
            if (colon_pos != std::string::npos) {
                server_address = server_info.substr(0, colon_pos);
                port = std::stoi(server_info.substr(colon_pos + 1));
            } else {
                server_address = server_info;
            }
        }
    }
    
    // Local mode
    if (local_mode) {
        std::cout << "🎮 Starting KungFu Chess in local mode..." << std::endl;
        
        try {
            auto img_factory = std::make_shared<OpenCvImgFactory>("KungFu Chess - Local Game");
            std::string pieces_root = "assets/client_assets/pieces/";
            
            auto game = create_game(pieces_root, img_factory);
            game.run(-1, true);
            
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "❌ Local game error: " << e.what() << std::endl;
            return 1;
        }
    }
    
    // Default to server connection - keep port 8080
    
    std::cout << "🎮 KungFu Chess Client connecting to " << server_address << ":" << port << "..." << std::endl;
    
    GameClient client(server_address, port);
    client.run();
    
    return 0;
}

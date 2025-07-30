#include <iostream>
#include <string>
#include "network/GameClient.hpp"

int main(int argc, char* argv[]) {
    std::string server_address = "127.0.0.1"; // localhost
    int port = 8080;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--server=") == 0) {
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
    
    std::cout << "🎮 KungFu Chess Client connecting to " << server_address << ":" << port << "..." << std::endl;
    
    GameClient client(server_address, port);
    client.run();
    
    return 0;
}

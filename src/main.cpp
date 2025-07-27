#include <iostream>
#include "Game.hpp"
#include "img/OpenCvImg.hpp"
#include <memory>

int main() {
    try {
        std::cout << "=== KFC Merged - Kung Fu Chess Game ===" << std::endl;
        std::cout << "Initializing OpenCV image factory..." << std::endl;
        
        auto img_factory = std::make_shared<OpenCvImgFactory>();
        std::string pieces_root = "pieces/";
        
        std::cout << "Creating game from pieces directory: " << pieces_root << std::endl;
        auto game = create_game(pieces_root, img_factory);
        
        std::cout << "Starting game with graphics enabled..." << std::endl;
        game.run(-1, true); // Run for 5 iterations with graphics
        
        std::cout << "Game completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
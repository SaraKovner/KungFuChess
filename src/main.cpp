#include <iostream>
#include "Game.hpp"
#include "img/OpenCvImg.hpp"
#include <memory>

int main() {
    try {
        auto img_factory = std::make_shared<OpenCvImgFactory>();
        std::string pieces_root = "pieces/";
        auto game = create_game(pieces_root, img_factory);
        game.run(-1, true);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/core/Game.hpp"
#include "../src/ui/Command.hpp"
#include "../src/core/Board.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "MockFactories.hpp"

TEST_CASE("Game construction") {
    // Create a simple board and pieces vector with one piece
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    // Create at least one piece to avoid "No pieces provided" error
    MockPieceFactory factory;
    std::vector<PiecePtr> pieces = { factory.create_simple_piece("PW1") };
    
    SECTION("Basic constructor") {
        REQUIRE_NOTHROW([&]() {
            Game game(pieces, board);
        }());
    }
}

TEST_CASE("Game basic operations") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    MockPieceFactory factory;
    std::vector<PiecePtr> pieces = { factory.create_simple_piece("PW1") };
    
    Game game(pieces, board);
    
    SECTION("Game time") {
        REQUIRE(game.game_time_ms() >= 0);
    }
    
    SECTION("Clone board") {
        REQUIRE_NOTHROW([&]() {
            auto cloned = game.clone_board();
        }());
    }
}

TEST_CASE("Game commands") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    MockPieceFactory factory;
    std::vector<PiecePtr> pieces = { factory.create_simple_piece("PW1") };
    
    Game game(pieces, board);
    
    SECTION("Enqueue command") {
        REQUIRE_NOTHROW([&]() {
            Command run_cmd(1000, "", "run", {});
            game.enqueue_command(run_cmd);
        }());
    }
    
    SECTION("Enqueue move command") {
        REQUIRE_NOTHROW([&]() {
            Command move_cmd(2000, "PW1", "move", {{1,1}, {2,2}});
            game.enqueue_command(move_cmd);
        }());
    }
}

TEST_CASE("Game with pieces") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    // Create some simple pieces for testing
    MockPieceFactory factory;
    std::vector<PiecePtr> pieces = { factory.create_simple_piece("PW1") };
    
    SECTION("One piece vector") {
        REQUIRE_NOTHROW([&]() {
            Game game(pieces, board);
            REQUIRE(game.pieces.size() == 1);
        }());
    }
}

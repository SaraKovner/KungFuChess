#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/core/Piece.hpp"
#include "../src/core/State.hpp"
#include "../src/core/Physics.hpp"
#include "../src/graphics/Graphics.hpp"
#include "../src/game_logic/Moves.hpp"
#include "../src/core/Board.hpp"
#include "../src/ui/Command.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "MockFactories.hpp"

TEST_CASE("Piece construction") {
    MockPieceFactory factory;
    
    SECTION("Basic piece creation") {
        REQUIRE_NOTHROW([&]() {
            auto piece = factory.create_simple_piece("PW1", "idle");
            REQUIRE(piece != nullptr);
            REQUIRE(piece->id == "PW1");
        }());
    }
    
    SECTION("Different piece types") {
        REQUIRE_NOTHROW([&]() {
            auto pawn = factory.create_simple_piece("PW1", "idle");
            auto king = factory.create_simple_piece("KW1", "idle");
            auto queen = factory.create_simple_piece("QB1", "idle");
        }());
    }
}

TEST_CASE("Piece state management") {
    MockPieceFactory factory;
    auto piece = factory.create_simple_piece("RW1", "idle");
    
    SECTION("Get current state") {
        REQUIRE(piece->state != nullptr);
        REQUIRE(piece->state->name == "idle");
    }
    
    SECTION("State access") {
        REQUIRE_NOTHROW([&]() {
            // Piece should have the state we created
            auto state = piece->state;
            REQUIRE(state != nullptr);
        }());
    }
}

TEST_CASE("Piece position and movement") {
    MockPieceFactory factory;
    auto piece = factory.create_simple_piece("NB1", "idle");
    
    SECTION("Get position") {
        REQUIRE_NOTHROW([&]() {
            auto cell = piece->current_cell();
            auto physics_pos = piece->state->physics->get_pos_m();
        }());
    }
    
    SECTION("Update piece") {
        REQUIRE_NOTHROW([&]() {
            piece->update(1000);
            piece->update(2000);
        }());
    }
    
    SECTION("Reset with time") {
        REQUIRE_NOTHROW([&]() {
            piece->reset(1500);
        }());
    }
}

TEST_CASE("Piece team identification") {
    MockPieceFactory factory;
    
    SECTION("White pieces") {
        auto white_pawn = factory.create_simple_piece("PW1", "idle");
        auto white_king = factory.create_simple_piece("KW1", "idle");
        
        REQUIRE(white_pawn->id.find("W") != std::string::npos);
        REQUIRE(white_king->id.find("W") != std::string::npos);
    }
    
    SECTION("Black pieces") {
        auto black_pawn = factory.create_simple_piece("PB1", "idle");
        auto black_queen = factory.create_simple_piece("QB1", "idle");
        
        REQUIRE(black_pawn->id.find("B") != std::string::npos);
        REQUIRE(black_queen->id.find("B") != std::string::npos);
    }
}

TEST_CASE("Piece rendering") {
    MockPieceFactory factory;
    auto piece = factory.create_simple_piece("QW1", "idle");
    
    SECTION("Access graphics") {
        REQUIRE_NOTHROW([&]() {
            auto graphics = piece->state->graphics;
            REQUIRE(graphics != nullptr);
        }());
    }
    
    SECTION("Graphics operations") {
        REQUIRE_NOTHROW([&]() {
            auto graphics = piece->state->graphics;
            graphics->update(1000);
        }());
    }
}

TEST_CASE("Piece with different physics") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    MockPieceFactory factory;
    
    SECTION("Piece with idle physics") {
        auto idle_physics = std::make_shared<IdlePhysics>(board);
        auto piece = factory.create_piece_with_physics("PW1", idle_physics);
        REQUIRE(piece != nullptr);
    }
    
    SECTION("Piece with move physics") {
        auto move_physics = std::make_shared<MovePhysics>(board, 1.0);
        auto piece = factory.create_piece_with_physics("RB1", move_physics);
        REQUIRE(piece != nullptr);
    }
    
    SECTION("Piece with jump physics") {
        auto jump_physics = std::make_shared<JumpPhysics>(board, 0.5);
        auto piece = factory.create_piece_with_physics("NW1", jump_physics);
        REQUIRE(piece != nullptr);
    }
}

#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/core/State.hpp"
#include "../src/core/Physics.hpp"
#include "../src/graphics/Graphics.hpp"
#include "../src/game_logic/Moves.hpp"
#include "../src/core/Board.hpp"
#include "../src/ui/Command.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "MockFactories.hpp"

TEST_CASE("State construction") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    // Create components
    auto moves = std::make_shared<Moves>("pieces/board.csv", std::make_pair(8, 8));
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    auto graphics = std::make_shared<Graphics>("pieces/PW", std::make_pair(50, 50), img_factory, true, 0.2);
    auto physics = std::make_shared<IdlePhysics>(board);
    
    SECTION("Basic construction") {
        REQUIRE_NOTHROW([&]() {
            State state(moves, graphics, physics);
        }());
    }
    
    SECTION("State with name") {
        REQUIRE_NOTHROW([&]() {
            State state(moves, graphics, physics);
            state.name = "idle";
            REQUIRE(state.name == "idle");
        }());
    }
}

TEST_CASE("State component access") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    auto moves = std::make_shared<Moves>("pieces/board.csv", std::make_pair(8, 8));
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    auto graphics = std::make_shared<Graphics>("pieces/KB", std::make_pair(50, 50), img_factory, true, 0.2);
    auto physics = std::make_shared<MovePhysics>(board, 1.5);
    
    State state(moves, graphics, physics);
    
    SECTION("Access moves") {
        REQUIRE(state.moves != nullptr);
    }
    
    SECTION("Access graphics") {
        REQUIRE(state.graphics != nullptr);
    }
    
    SECTION("Access physics") {
        REQUIRE(state.physics != nullptr);
    }
}

TEST_CASE("State with different physics types") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    auto moves = std::make_shared<Moves>("pieces/board.csv", std::make_pair(8, 8));
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    auto graphics = std::make_shared<Graphics>("pieces/QW", std::make_pair(50, 50), img_factory, true, 0.2);
    
    SECTION("State with idle physics") {
        auto idle_physics = std::make_shared<IdlePhysics>(board);
        REQUIRE_NOTHROW([&]() {
            State state(moves, graphics, idle_physics);
            state.name = "idle";
        }());
    }
    
    SECTION("State with move physics") {
        auto move_physics = std::make_shared<MovePhysics>(board, 2.0);
        REQUIRE_NOTHROW([&]() {
            State state(moves, graphics, move_physics);
            state.name = "move";
        }());
    }
    
    SECTION("State with jump physics") {
        auto jump_physics = std::make_shared<JumpPhysics>(board, 0.3);
        REQUIRE_NOTHROW([&]() {
            State state(moves, graphics, jump_physics);
            state.name = "jump";
        }());
    }
}

TEST_CASE("State operations") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    auto moves = std::make_shared<Moves>("pieces/board.csv", std::make_pair(8, 8));
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    auto graphics = std::make_shared<Graphics>("pieces/RB", std::make_pair(50, 50), img_factory, true, 0.2);
    auto physics = std::make_shared<IdlePhysics>(board);
    
    State state(moves, graphics, physics);
    state.name = "test_state";
    
    SECTION("State name setting") {
        REQUIRE(state.name == "test_state");
        state.name = "new_name";
        REQUIRE(state.name == "new_name");
    }
    
    SECTION("Component consistency") {
        // All components should remain accessible
        REQUIRE(state.moves != nullptr);
        REQUIRE(state.graphics != nullptr);
        REQUIRE(state.physics != nullptr);
    }
}

TEST_CASE("State with mock factory") {
    MockPieceFactory factory;
    
    SECTION("Create state through factory") {
        REQUIRE_NOTHROW([&]() {
            auto piece = factory.create_simple_piece("NW1", "custom_state");
            auto state = piece->state;
            REQUIRE(state != nullptr);
            REQUIRE(state->name == "custom_state");
        }());
    }
}

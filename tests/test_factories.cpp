#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/game_logic/PhysicsFactory.hpp"
#include "../src/game_logic/PieceFactory.hpp"
#include "../src/graphics/GraphicsFactory.hpp"
#include "../src/core/Physics.hpp"
#include "../src/core/Board.hpp"
#include "../src/ui/Command.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "MockFactories.hpp"
#include "../src/utils/json/nlohmann/json.hpp"

TEST_CASE("PhysicsFactory") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    PhysicsFactory factory(board);
    
    SECTION("Create idle physics") {
        REQUIRE_NOTHROW([&]() {
            auto physics = factory.create({0,0}, "idle", {});
            REQUIRE(physics != nullptr);
        }());
    }
    
    SECTION("Create move physics") {
        nlohmann::json config;
        config["speed_m_per_sec"] = 1.5;
        REQUIRE_NOTHROW([&]() {
            auto physics = factory.create({0,0}, "move", config);
            REQUIRE(physics != nullptr);
        }());
    }
    
    SECTION("Create jump physics") {
        nlohmann::json config;
        config["duration_ms"] = 750;
        REQUIRE_NOTHROW([&]() {
            auto physics = factory.create({0,0}, "jump", config);
            REQUIRE(physics != nullptr);
        }());
    }
    
    SECTION("Default configurations") {
        REQUIRE_NOTHROW([&]() {
            auto idle = factory.create({0,0}, "idle", {});
            auto move = factory.create({0,0}, "move", {});
            auto jump = factory.create({0,0}, "jump", {});
        }());
    }
    
    SECTION("Case insensitive") {
        REQUIRE_NOTHROW([&]() {
            auto idle1 = factory.create({0,0}, "IDLE", {});
            auto idle2 = factory.create({0,0}, "Idle", {});
            auto move1 = factory.create({0,0}, "MOVE", {});
            auto move2 = factory.create({0,0}, "Move", {});
        }());
    }
}

TEST_CASE("GraphicsFactory") {
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    GraphicsFactory factory(img_factory);
    
    SECTION("Basic graphics creation") {
        nlohmann::json config;
        config["is_loop"] = true;
        config["frames_per_sec"] = 3.0;
        REQUIRE_NOTHROW([&]() {
            auto graphics = factory.load("../assets/pieces/PW", config, {50, 50});
            REQUIRE(graphics != nullptr);
        }());
    }
    
    SECTION("Different piece sprites") {
        nlohmann::json config;
        config["is_loop"] = true;
        config["frames_per_sec"] = 3.0;
        REQUIRE_NOTHROW([&]() {
            auto pawn = factory.load("../assets/pieces/PW", config, {50, 50});
            auto king = factory.load("../assets/pieces/KW", config, {60, 60});
            auto queen = factory.load("../assets/pieces/QB", config, {55, 55});
        }());
    }
    
    SECTION("Different cell sizes") {
        nlohmann::json config;
        config["is_loop"] = true;
        config["frames_per_sec"] = 3.0;
        REQUIRE_NOTHROW([&]() {
            auto small = factory.load("../assets/pieces/RW", config, {30, 30});
            auto medium = factory.load("../assets/pieces/RW", config, {50, 50});
            auto large = factory.load("../assets/pieces/RW", config, {80, 80});
        }());
    }
}

TEST_CASE("MockPhysicsFactory") {
    MockPhysicsFactory factory;
    
    SECTION("Create physics objects") {
        REQUIRE_NOTHROW([&]() {
            auto idle = factory.create({0,0}, "idle", {});
            auto move = factory.create({0,0}, "move", {});
            auto jump = factory.create({0,0}, "jump", {});
            REQUIRE(idle != nullptr);
            REQUIRE(move != nullptr);
            REQUIRE(jump != nullptr);
        }());
    }
    
    SECTION("Default behavior") {
        REQUIRE_NOTHROW([&]() {
            auto physics1 = factory.create({0,0}, "unknown", {});
            auto physics2 = factory.create({0,0}, "", {});
            REQUIRE(physics1 != nullptr);
            REQUIRE(physics2 != nullptr);
        }());
    }
}

TEST_CASE("MockPieceFactory") {
    MockPieceFactory factory;
    
    SECTION("Create simple pieces") {
        REQUIRE_NOTHROW([&]() {
            auto piece1 = factory.create_simple_piece("PW1");
            auto piece2 = factory.create_simple_piece("KB1", "move");
            REQUIRE(piece1 != nullptr);
            REQUIRE(piece2 != nullptr);
            REQUIRE(piece1->id == "PW1");
            REQUIRE(piece2->id == "KB1");
        }());
    }
    
    SECTION("Create pieces with custom physics") {
        ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
        Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
        
        REQUIRE_NOTHROW([&]() {
            auto physics = std::make_shared<MovePhysics>(board, 2.0);
            auto piece = factory.create_piece_with_physics("QW1", physics, "move");
            REQUIRE(piece != nullptr);
            REQUIRE(piece->id == "QW1");
        }());
    }
}

TEST_CASE("MockGraphicsFactory") {
    MockGraphicsFactory factory;
    
    SECTION("Create graphics objects") {
        REQUIRE_NOTHROW([&]() {
            auto graphics = factory.create_graphics("pieces/NB", {45, 45});
            REQUIRE(graphics != nullptr);
        }());
    }
    
    SECTION("Different configurations") {
        REQUIRE_NOTHROW([&]() {
            auto g1 = factory.create_graphics("pieces/PW", {50, 50});
            auto g2 = factory.create_graphics("pieces/KB", {60, 60});
            auto g3 = factory.create_graphics("pieces/QW", {40, 40});
        }());
    }
}

TEST_CASE("Factory integration") {
    SECTION("All factories work together") {
        MockPhysicsFactory physics_factory;
        MockPieceFactory piece_factory;
        MockGraphicsFactory graphics_factory;
        
        REQUIRE_NOTHROW([&]() {
            // Create physics
            auto physics = physics_factory.create({0,0}, "move", {});
            
            // Create graphics
            auto graphics = graphics_factory.create_graphics("pieces/RW", {50, 50});
            
            // Create piece
            auto piece = piece_factory.create_simple_piece("RW1", "move");
            
            REQUIRE(physics != nullptr);
            REQUIRE(graphics != nullptr);
            REQUIRE(piece != nullptr);
        }());
    }
}

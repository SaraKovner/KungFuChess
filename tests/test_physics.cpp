#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/core/Physics.hpp"
#include "../src/core/Board.hpp"
#include "../src/ui/Command.hpp"
#include "../src/graphics/img/MockImg.hpp"

TEST_CASE("IdlePhysics") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Basic construction") {
        REQUIRE_NOTHROW([&]() {
            IdlePhysics physics(board);
        }());
    }
    
    SECTION("Reset command") {
        IdlePhysics physics(board);
        Command cmd(1000, "PW1", "idle", {});
        REQUIRE_NOTHROW([&]() {
            physics.reset(cmd);
        }());
        REQUIRE(physics.start_ms == 1000);
    }
    
    SECTION("Update method") {
        IdlePhysics physics(board);
        Command cmd(500, "PW1", "idle", {});
        physics.reset(cmd);
        REQUIRE_NOTHROW([&]() {
            physics.update(1000);
            physics.update(2000);
        }());
    }
    
    SECTION("Get position") {
        IdlePhysics physics(board);
        REQUIRE_NOTHROW([&]() {
            auto pos = physics.get_pos_m();
            auto pix = physics.get_pos_pix();
            auto cell = physics.get_curr_cell();
        }());
    }
}

TEST_CASE("MovePhysics") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Basic construction") {
        REQUIRE_NOTHROW([&]() {
            MovePhysics physics(board, 1.0); // 1 meter per second
        }());
    }
    
    SECTION("Construction with different speeds") {
        REQUIRE_NOTHROW([&]() {
            MovePhysics fast(board, 2.0);
            MovePhysics slow(board, 0.5);
        }());
    }
    
    SECTION("Reset with move command") {
        MovePhysics physics(board, 1.0);
        Command move_cmd(1000, "PW1", "move", {{0,0}, {1,1}});
        REQUIRE_NOTHROW([&]() {
            physics.reset(move_cmd);
        }());
        REQUIRE(physics.start_ms == 1000);
    }
    
    SECTION("Duration calculation") {
        MovePhysics physics(board, 1.0); // 1 cell per second
        Command move_cmd(1000, "PW1", "move", {{0,0}, {1,0}}); // 1 cell move
        physics.reset(move_cmd);
        
        // Check that physics was properly initialized
        REQUIRE(physics.get_start_ms() == 1000);
        REQUIRE_NOTHROW([&]() {
            physics.update(1500);
        }());
    }
    
    SECTION("Update during movement") {
        MovePhysics physics(board, 1.0);
        Command move_cmd(1000, "PW1", "move", {{0,0}, {2,0}});
        physics.reset(move_cmd);
        
        REQUIRE_NOTHROW([&]() {
            physics.update(1500); // Mid-movement
            physics.update(3000); // After movement
        }());
    }
}

TEST_CASE("JumpPhysics") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Basic construction") {
        REQUIRE_NOTHROW([&]() {
            JumpPhysics physics(board, 0.5); // 0.5 second duration
        }());
    }
    
    SECTION("Different durations") {
        REQUIRE_NOTHROW([&]() {
            JumpPhysics quick(board, 0.1);
            JumpPhysics slow(board, 1.0);
        }());
    }
    
    SECTION("Reset with jump command") {
        JumpPhysics physics(board, 0.5);
        Command jump_cmd(2000, "NW1", "jump", {{1,1}, {3,2}});
        REQUIRE_NOTHROW([&]() {
            physics.reset(jump_cmd);
        }());
        REQUIRE(physics.start_ms == 2000);
    }
    
    SECTION("Duration method") {
        JumpPhysics physics(board, 0.75);
        Command jump_cmd(1000, "NW1", "jump", {{0,0}, {2,1}});
        physics.reset(jump_cmd);
        
        // Check that physics was properly initialized
        REQUIRE(physics.get_start_ms() == 1000);
        REQUIRE_NOTHROW([&]() {
            physics.update(1375); // Mid-jump
        }());
    }
    
    SECTION("Update during jump") {
        JumpPhysics physics(board, 0.5);
        Command jump_cmd(1000, "NW1", "jump", {{0,0}, {2,1}});
        physics.reset(jump_cmd);
        
        REQUIRE_NOTHROW([&]() {
            physics.update(1250); // Mid-jump
            physics.update(1500); // End of jump
            physics.update(2000); // After jump
        }());
    }
}

TEST_CASE("Physics inheritance") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Polymorphic usage") {
        std::shared_ptr<BasePhysics> idle = std::make_shared<IdlePhysics>(board);
        std::shared_ptr<BasePhysics> move = std::make_shared<MovePhysics>(board, 1.0);
        std::shared_ptr<BasePhysics> jump = std::make_shared<JumpPhysics>(board, 0.5);
        
        REQUIRE_NOTHROW([&]() {
            Command cmd(1000, "test", "action", {{0,0}});
            idle->reset(cmd);
            Command move_cmd(1000, "test", "move", {{0,0}, {1,1}});
            move->reset(move_cmd);
            Command jump_cmd(1000, "test", "jump", {{0,0}});
            jump->reset(jump_cmd);
        }());
    }
}

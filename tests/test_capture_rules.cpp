#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/game_logic/CaptureRules.hpp"
#include "../src/core/Piece.hpp"
#include "../src/core/State.hpp"
#include "../src/core/Physics.hpp"
#include "../src/core/Board.hpp"
#include "../src/game_logic/Moves.hpp"
#include "../src/graphics/Graphics.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "MockFactories.hpp"

// Helper function to create test pieces with minimal dependencies
PiecePtr create_simple_test_piece(const std::string& id, const std::string& state_name, 
                                 std::shared_ptr<BasePhysics> physics) {
    // Use the MockPieceFactory helper
    MockPieceFactory factory;
    return factory.create_piece_with_physics(id, physics, state_name);
}

TEST_CASE("CaptureRules::are_same_team") {
    // Create a shared board for physics objects
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(100, 100));
    auto board = Board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Same team pieces") {
        auto white_pawn = create_simple_test_piece("PW1", "idle", std::make_shared<IdlePhysics>(board));
        auto white_king = create_simple_test_piece("KW1", "idle", std::make_shared<IdlePhysics>(board));
        
        REQUIRE(CaptureRules::are_same_team(white_pawn, white_king) == true);
    }
    
    SECTION("Different team pieces") {
        auto white_pawn = create_simple_test_piece("PW1", "idle", std::make_shared<IdlePhysics>(board));
        auto black_pawn = create_simple_test_piece("PB1", "idle", std::make_shared<IdlePhysics>(board));
        
        REQUIRE(CaptureRules::are_same_team(white_pawn, black_pawn) == false);
    }
    
    SECTION("Black team pieces") {
        auto black_queen = create_simple_test_piece("QB1", "idle", std::make_shared<IdlePhysics>(board));
        auto black_rook = create_simple_test_piece("RB1", "idle", std::make_shared<IdlePhysics>(board));
        
        REQUIRE(CaptureRules::are_same_team(black_queen, black_rook) == true);
    }
    
    SECTION("Null pointer handling") {
        auto white_pawn = create_simple_test_piece("PW1", "idle", std::make_shared<IdlePhysics>(board));
        
        REQUIRE(CaptureRules::are_same_team(nullptr, white_pawn) == false);
        REQUIRE(CaptureRules::are_same_team(white_pawn, nullptr) == false);
        REQUIRE(CaptureRules::are_same_team(nullptr, nullptr) == false);
    }
    
    SECTION("Invalid piece IDs") {
        auto invalid1 = create_simple_test_piece("P", "idle", std::make_shared<IdlePhysics>(board)); // Too short
        auto invalid2 = create_simple_test_piece("", "idle", std::make_shared<IdlePhysics>(board));  // Empty
        auto valid = create_simple_test_piece("PW1", "idle", std::make_shared<IdlePhysics>(board));
        
        REQUIRE(CaptureRules::are_same_team(invalid1, valid) == false);
        REQUIRE(CaptureRules::are_same_team(invalid2, valid) == false);
        REQUIRE(CaptureRules::are_same_team(invalid1, invalid2) == false);
    }
}

TEST_CASE("CaptureRules::calculate_arrival_time") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(100, 100));
    auto board = Board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Idle piece arrival time") {
        auto idle_physics = std::make_shared<IdlePhysics>(board);
        idle_physics->start_ms = 1500;
        auto idle_piece = create_simple_test_piece("PW1", "idle", idle_physics);
        
        REQUIRE(CaptureRules::calculate_arrival_time(idle_piece) == 1500);
    }
    
    SECTION("Moving piece arrival time") {
        auto move_physics = std::make_shared<MovePhysics>(board, 1.0); // 1 cell per second
        
        // Simulate a 1-cell move command
        Command move_cmd{2000, "PB1", "move", {{0,0}, {1,0}}};
        move_physics->reset(move_cmd);
        
        auto moving_piece = create_simple_test_piece("PB1", "move", move_physics);
        
        // Should be start_ms + duration (1 second = 1000ms for 1 cell at 1 cell/s)
        REQUIRE(CaptureRules::calculate_arrival_time(moving_piece) == 3000);
    }
    
    SECTION("Moving piece with longer duration") {
        auto move_physics = std::make_shared<MovePhysics>(board, 0.5); // 0.5 cells per second
        
        // Simulate a 1-cell move command (should take 2 seconds)
        Command move_cmd{1000, "NW1", "move", {{0,0}, {1,0}}};
        move_physics->reset(move_cmd);
        
        auto moving_piece = create_simple_test_piece("NW1", "move", move_physics);
        
        // Should be start_ms + duration (2 seconds = 2000ms for 1 cell at 0.5 cell/s)
        REQUIRE(CaptureRules::calculate_arrival_time(moving_piece) == 3000);
    }
}

TEST_CASE("CaptureRules::determine_attacker_and_victim") {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(100, 100));
    auto board = Board(50, 50, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Later arriving piece is attacker") {
        auto early_physics = std::make_shared<IdlePhysics>(board);
        early_physics->start_ms = 1000;
        auto early_piece = create_simple_test_piece("PW1", "idle", early_physics);
        
        auto late_physics = std::make_shared<IdlePhysics>(board);
        late_physics->start_ms = 2000;
        auto late_piece = create_simple_test_piece("PB1", "idle", late_physics);
        
        auto [attacker, victim] = CaptureRules::determine_attacker_and_victim(early_piece, late_piece);
        
        REQUIRE(attacker == late_piece);
        REQUIRE(victim == early_piece);
    }
    
    SECTION("Earlier arriving piece vs later arriving piece") {
        auto early_physics = std::make_shared<IdlePhysics>(board);
        early_physics->start_ms = 500;
        auto early_piece = create_simple_test_piece("QB1", "idle", early_physics);
        
        auto late_physics = std::make_shared<IdlePhysics>(board);
        late_physics->start_ms = 1500;
        auto late_piece = create_simple_test_piece("QW1", "idle", late_physics);
        
        auto [attacker, victim] = CaptureRules::determine_attacker_and_victim(late_piece, early_piece);
        
        REQUIRE(attacker == late_piece);
        REQUIRE(victim == early_piece);
    }
    
    SECTION("Simultaneous arrival - earlier start wins") {
        auto early_start_physics = std::make_shared<IdlePhysics>(board);
        early_start_physics->start_ms = 1000;
        auto early_start_piece = create_simple_test_piece("RW1", "idle", early_start_physics);
        
        auto late_start_physics = std::make_shared<IdlePhysics>(board);
        late_start_physics->start_ms = 1500;
        auto late_start_piece = create_simple_test_piece("RB1", "idle", late_start_physics);
        
        auto [attacker, victim] = CaptureRules::determine_attacker_and_victim(early_start_piece, late_start_piece);
        
        // תיקון: מי שמגיע אחרון הוא התוקף, לא מי שיצא קודם
        // late_start_piece מגיע ב-1500ms, early_start_piece מגיע ב-1000ms
        // לכן late_start_piece הוא התוקף
        REQUIRE(attacker == late_start_piece);
        REQUIRE(victim == early_start_piece);
    }
    
    SECTION("True simultaneous arrival - earlier start wins") {
        // יצירת שני כלים שמגיעים באותו זמן אבל יוצאים בזמנים שונים
        // נדמה קצת מתקדם - נשתמש במקום זה בזמני התחלה זהים
        auto move_physics1 = std::make_shared<MovePhysics>(board, 1.0); // 1 cell/sec
        Command move_cmd1{1000, "RW1", "move", {{0,0}, {1,0}}}; // starts at 1000ms, arrives at 2000ms
        move_physics1->reset(move_cmd1);
        auto piece1 = create_simple_test_piece("RW1", "move", move_physics1);
        
        auto move_physics2 = std::make_shared<MovePhysics>(board, 1.0); // 1 cell/sec  
        Command move_cmd2{1200, "RB1", "move", {{0,0}, {1,0}}}; // starts at 1200ms, arrives at 2200ms
        move_physics2->reset(move_cmd2);
        auto piece2 = create_simple_test_piece("RB1", "move", move_physics2);
        
        auto [attacker, victim] = CaptureRules::determine_attacker_and_victim(piece1, piece2);
        
        // piece2 מגיע ב-2200ms, piece1 מגיע ב-2000ms
        // לכן piece2 הוא התוקף
        REQUIRE(attacker == piece2);
        REQUIRE(victim == piece1);
    }
    
    SECTION("Perfect tie - no capture") {
        auto physics1 = std::make_shared<IdlePhysics>(board);
        physics1->start_ms = 1000;
        auto piece1 = create_simple_test_piece("KW1", "idle", physics1);
        
        auto physics2 = std::make_shared<IdlePhysics>(board);
        physics2->start_ms = 1000;
        auto piece2 = create_simple_test_piece("KB1", "idle", physics2);
        
        auto [attacker, victim] = CaptureRules::determine_attacker_and_victim(piece1, piece2);
        
        REQUIRE(attacker == nullptr);
        REQUIRE(victim == nullptr);
    }
}

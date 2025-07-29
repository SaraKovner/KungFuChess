#pragma once

#include "../src/core/Common.hpp"
#include "../src/game_logic/PhysicsFactory.hpp"
#include "../src/core/Physics.hpp"
#include "../src/game_logic/PieceFactory.hpp"
#include "../src/graphics/GraphicsFactory.hpp"
#include "../src/core/Piece.hpp"
#include "../src/graphics/Graphics.hpp"
#include "../src/core/Board.hpp"
#include "../src/game_logic/Moves.hpp"
#include "../src/core/State.hpp"
#include "../src/graphics/img/MockImg.hpp"

// Helper function to create a mock board
inline Board create_mock_board() {
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    return Board(50, 50, 8, 8, mock_img, 1.0, 1.0);
}

class MockPhysicsFactory : public PhysicsFactory {
private:
    Board mock_board;
    
public:
    MockPhysicsFactory() : PhysicsFactory(create_mock_board()), mock_board(create_mock_board()) {}
    
    std::shared_ptr<BasePhysics> create(const std::pair<int,int>& /*start_cell*/,
                                        const std::string& name,
                                        const nlohmann::json& cfg) const {
        std::string key = name;
        if(key == "idle" || key.empty()) {
            return std::make_shared<IdlePhysics>(mock_board);
        }
        if(key == "move") {
            double speed = cfg.is_null() ? 1.0 : cfg.value("speed_m_per_sec", 1.0);
            return std::make_shared<MovePhysics>(mock_board, speed);
        }
        // Default to idle
        return std::make_shared<IdlePhysics>(mock_board);
    }
};

class MockPieceFactory {
public:
    MockPieceFactory() {}
    
    // Simple helper for creating test pieces
    PiecePtr create_simple_piece(const std::string& id, const std::string& state_name = "idle") {
        Board board = create_mock_board();
        auto physics = std::make_shared<IdlePhysics>(board);
        physics->start_ms = 0; // Set default start time
        
        // Create Moves with a simple moves file
        auto moves = std::make_shared<Moves>("pieces/board.csv", std::make_pair(8, 8));
        
        // Create Graphics
        ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
        GraphicsPtr graphics = std::make_shared<Graphics>("pieces/PW", std::make_pair(50, 50), img_factory, true, 0.2);
        
        // Create State
        StatePtr state = std::make_shared<State>(moves, graphics, physics);
        state->name = state_name;
        
        return std::make_shared<Piece>(id, state);
    }
    
    // Helper for creating piece with specific physics and start time
    PiecePtr create_piece_with_physics(const std::string& id, std::shared_ptr<BasePhysics> physics, const std::string& state_name = "idle") {
        // Create Moves with a simple moves file
        auto moves = std::make_shared<Moves>("pieces/board.csv", std::make_pair(8, 8));
        
        // Create Graphics
        ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
        GraphicsPtr graphics = std::make_shared<Graphics>("pieces/PW", std::make_pair(50, 50), img_factory, true, 0.2);
        
        // Create State
        StatePtr state = std::make_shared<State>(moves, graphics, physics);
        state->name = state_name;
        
        return std::make_shared<Piece>(id, state);
    }
};

class MockGraphicsFactory {
public:
    MockGraphicsFactory() {}
    
    GraphicsPtr create_graphics(const std::string& sprites_dir, std::pair<int,int> cell_size) {
        ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
        return std::make_shared<Graphics>(sprites_dir, cell_size, img_factory, true, 0.2);
    }
};

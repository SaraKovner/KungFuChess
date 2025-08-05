#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_set>
#include "Common.hpp"
#include "ServerMoves.hpp"

// Simple server-side piece representation
struct ServerPiece {
    std::string id;
    std::pair<int, int> position;
    char color; // 'W' or 'B'
    char type;  // 'P', 'R', 'N', 'B', 'Q', 'K'
    std::shared_ptr<ServerMoves> moves; // Move rules for this piece
    
    ServerPiece(const std::string& piece_id, int x, int y) 
        : id(piece_id), position({x, y}) {
        if (piece_id.length() >= 2) {
            type = piece_id[0];
            color = piece_id[1];
        }
    }
};

using ServerPiecePtr = std::shared_ptr<ServerPiece>;

// Simple server-side board
class ServerBoard {
public:
    static const int WIDTH = 8;
    static const int HEIGHT = 8;
    
    std::unordered_map<std::pair<int,int>, ServerPiecePtr, PairHash> pieces_at_position;
    
    ServerPiecePtr getPieceAt(int x, int y) {
        auto it = pieces_at_position.find({x, y});
        return (it != pieces_at_position.end()) ? it->second : nullptr;
    }
    
    void setPieceAt(int x, int y, ServerPiecePtr piece) {
        if (piece) {
            pieces_at_position[{x, y}] = piece;
            piece->position = {x, y};
        } else {
            pieces_at_position.erase({x, y});
        }
    }
    
    void removePieceAt(int x, int y) {
        pieces_at_position.erase({x, y});
    }
    
    bool isValidPosition(int x, int y) {
        return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
    }
};

// Server-side game engine
class ServerGameEngine {
private:
    ServerBoard board_;
    
    // Player states
    struct PlayerState {
        std::pair<int, int> cursor_pos = {0, 0};
        ServerPiecePtr selected_piece = nullptr;
        std::pair<int, int> selected_piece_pos = {-1, -1};
        bool has_selected_piece = false;
    };
    
    PlayerState player1_state_; // White player
    PlayerState player2_state_; // Black player
    
public:
    ServerGameEngine();
    
    // Initialize standard chess starting position
    void initializeStandardGame();
    
    // Player state management
    void updatePlayerCursor(int player_id, const std::string& direction);
    std::pair<int, int> getPlayerCursor(int player_id);
    
    // Piece selection logic
    bool processSelect(int player_id);
    ServerPiecePtr getSelectedPiece(int player_id);
    
    // Board queries
    ServerPiecePtr getPieceAt(int x, int y);
    bool canPlayerControlPiece(int player_id, ServerPiecePtr piece);
    
    // Move validation (basic for now)
    bool isValidMove(ServerPiecePtr piece, std::pair<int,int> from, std::pair<int,int> to);
    
    // Debug
    void printBoardState();
    
private:
    // Helper to create moves for piece type
    std::shared_ptr<ServerMoves> createMovesForPiece(char piece_type);
};
#include "ServerGameEngine.hpp"

ServerGameEngine::ServerGameEngine() {
    // Initialize player starting positions
    player1_state_.cursor_pos = {7, 7}; // White starts bottom-right
    player2_state_.cursor_pos = {0, 0}; // Black starts top-left
}

void ServerGameEngine::initializeStandardGame() {
    std::cout << "🎮 Initializing standard chess game..." << std::endl;
    
    // Clear board
    board_.pieces_at_position.clear();
    
    // White pieces (bottom rows)
    auto rw1 = std::make_shared<ServerPiece>("RW", 0, 7);
    rw1->moves = createMovesForPiece('R');
    board_.setPieceAt(0, 7, rw1);
    
    auto nw1 = std::make_shared<ServerPiece>("NW", 1, 7);
    nw1->moves = createMovesForPiece('N');
    board_.setPieceAt(1, 7, nw1);
    
    auto bw1 = std::make_shared<ServerPiece>("BW", 2, 7);
    bw1->moves = createMovesForPiece('B');
    board_.setPieceAt(2, 7, bw1);
    
    auto qw = std::make_shared<ServerPiece>("QW", 3, 7);
    qw->moves = createMovesForPiece('Q');
    board_.setPieceAt(3, 7, qw);
    
    auto kw = std::make_shared<ServerPiece>("KW", 4, 7);
    kw->moves = createMovesForPiece('K');
    board_.setPieceAt(4, 7, kw);
    
    auto bw2 = std::make_shared<ServerPiece>("BW", 5, 7);
    bw2->moves = createMovesForPiece('B');
    board_.setPieceAt(5, 7, bw2);
    
    auto nw2 = std::make_shared<ServerPiece>("NW", 6, 7);
    nw2->moves = createMovesForPiece('N');
    board_.setPieceAt(6, 7, nw2);
    
    auto rw2 = std::make_shared<ServerPiece>("RW", 7, 7);
    rw2->moves = createMovesForPiece('R');
    board_.setPieceAt(7, 7, rw2);
    
    // White pawns
    for (int x = 0; x < 8; x++) {
        auto pw = std::make_shared<ServerPiece>("PW", x, 6);
        pw->moves = createMovesForPiece('P');
        board_.setPieceAt(x, 6, pw);
    }
    
    // Black pieces (top rows)
    auto rb1 = std::make_shared<ServerPiece>("RB", 0, 0);
    rb1->moves = createMovesForPiece('R');
    board_.setPieceAt(0, 0, rb1);
    
    auto nb1 = std::make_shared<ServerPiece>("NB", 1, 0);
    nb1->moves = createMovesForPiece('N');
    board_.setPieceAt(1, 0, nb1);
    
    auto bb1 = std::make_shared<ServerPiece>("BB", 2, 0);
    bb1->moves = createMovesForPiece('B');
    board_.setPieceAt(2, 0, bb1);
    
    auto qb = std::make_shared<ServerPiece>("QB", 3, 0);
    qb->moves = createMovesForPiece('Q');
    board_.setPieceAt(3, 0, qb);
    
    auto kb = std::make_shared<ServerPiece>("KB", 4, 0);
    kb->moves = createMovesForPiece('K');
    board_.setPieceAt(4, 0, kb);
    
    auto bb2 = std::make_shared<ServerPiece>("BB", 5, 0);
    bb2->moves = createMovesForPiece('B');
    board_.setPieceAt(5, 0, bb2);
    
    auto nb2 = std::make_shared<ServerPiece>("NB", 6, 0);
    nb2->moves = createMovesForPiece('N');
    board_.setPieceAt(6, 0, nb2);
    
    auto rb2 = std::make_shared<ServerPiece>("RB", 7, 0);
    rb2->moves = createMovesForPiece('R');
    board_.setPieceAt(7, 0, rb2);
    
    // Black pawns
    for (int x = 0; x < 8; x++) {
        auto pb = std::make_shared<ServerPiece>("PB", x, 1);
        pb->moves = createMovesForPiece('P');
        board_.setPieceAt(x, 1, pb);
    }
    
    std::cout << "✅ Standard chess game initialized" << std::endl;
}

void ServerGameEngine::updatePlayerCursor(int player_id, const std::string& direction) {
    PlayerState& state = (player_id == 1) ? player1_state_ : player2_state_;
    
    int dx = 0, dy = 0;
    if (direction == "up") dy = -1;
    else if (direction == "down") dy = 1;
    else if (direction == "left") dx = -1;
    else if (direction == "right") dx = 1;
    
    int new_x = state.cursor_pos.first + dx;
    int new_y = state.cursor_pos.second + dy;
    
    if (board_.isValidPosition(new_x, new_y)) {
        state.cursor_pos = {new_x, new_y};
        std::cout << "🎯 Player " << player_id << " cursor moved to (" << new_x << "," << new_y << ")" << std::endl;
    }
}

std::pair<int, int> ServerGameEngine::getPlayerCursor(int player_id) {
    PlayerState& state = (player_id == 1) ? player1_state_ : player2_state_;
    return state.cursor_pos;
}

bool ServerGameEngine::processSelect(int player_id) {
    PlayerState& state = (player_id == 1) ? player1_state_ : player2_state_;
    auto cursor_pos = state.cursor_pos;
    
    std::cout << "🎯 Processing SELECT for player " << player_id << " at (" << cursor_pos.first << "," << cursor_pos.second << ")" << std::endl;
    
    // Get piece at cursor position
    auto piece_at_cursor = board_.getPieceAt(cursor_pos.first, cursor_pos.second);
    
    if (!state.has_selected_piece) {
        // First SELECT - try to select a piece
        if (piece_at_cursor && canPlayerControlPiece(player_id, piece_at_cursor)) {
            state.selected_piece = piece_at_cursor;
            state.selected_piece_pos = cursor_pos;
            state.has_selected_piece = true;
            std::cout << "✅ Player " << player_id << " selected piece " << piece_at_cursor->id << std::endl;
            return true;
        } else {
            std::cout << "❌ Player " << player_id << " cannot select piece at cursor" << std::endl;
            return false;
        }
    } else {
        // Second SELECT - try to move or deselect
        if (cursor_pos == state.selected_piece_pos) {
            // Same position - deselect
            std::cout << "🔄 Player " << player_id << " deselected piece" << std::endl;
            state.selected_piece = nullptr;
            state.selected_piece_pos = {-1, -1};
            state.has_selected_piece = false;
            return true;
        } else {
            // Different position - try to move
            if (isValidMove(state.selected_piece, state.selected_piece_pos, cursor_pos)) {
                std::cout << "✅ Player " << player_id << " moving " << state.selected_piece->id 
                         << " from (" << state.selected_piece_pos.first << "," << state.selected_piece_pos.second 
                         << ") to (" << cursor_pos.first << "," << cursor_pos.second << ")" << std::endl;
                
                // Execute move
                board_.removePieceAt(state.selected_piece_pos.first, state.selected_piece_pos.second);
                board_.setPieceAt(cursor_pos.first, cursor_pos.second, state.selected_piece);
                
                // Reset selection
                state.selected_piece = nullptr;
                state.selected_piece_pos = {-1, -1};
                state.has_selected_piece = false;
                return true;
            } else {
                std::cout << "❌ Invalid move for player " << player_id << std::endl;
                return false;
            }
        }
    }
}

ServerPiecePtr ServerGameEngine::getSelectedPiece(int player_id) {
    PlayerState& state = (player_id == 1) ? player1_state_ : player2_state_;
    return state.selected_piece;
}

ServerPiecePtr ServerGameEngine::getPieceAt(int x, int y) {
    return board_.getPieceAt(x, y);
}

bool ServerGameEngine::canPlayerControlPiece(int player_id, ServerPiecePtr piece) {
    if (!piece) return false;
    
    char expected_color = (player_id == 1) ? 'W' : 'B';
    return piece->color == expected_color;
}

bool ServerGameEngine::isValidMove(ServerPiecePtr piece, std::pair<int,int> from, std::pair<int,int> to) {
    if (!piece) return false;
    if (!board_.isValidPosition(to.first, to.second)) return false;
    if (from == to) return false;
    
    // Check if destination has piece of same color
    auto dest_piece = board_.getPieceAt(to.first, to.second);
    if (dest_piece && dest_piece->color == piece->color) {
        return false; // Can't capture own piece
    }
    
    // Use piece-specific move validation if available
    if (piece->moves) {
        // Create set of occupied positions
        std::unordered_set<std::pair<int,int>, PairHash> occupied_cells;
        for (const auto& [pos, p] : board_.pieces_at_position) {
            occupied_cells.insert(pos);
        }
        
        return piece->moves->is_valid(from, to, occupied_cells);
    }
    
    // Fallback: basic validation
    return true;
}

void ServerGameEngine::printBoardState() {
    std::cout << "📋 Current board state:" << std::endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            auto piece = board_.getPieceAt(x, y);
            if (piece) {
                std::cout << piece->id << " ";
            } else {
                std::cout << ".. ";
            }
        }
        std::cout << std::endl;
    }
}

std::shared_ptr<ServerMoves> ServerGameEngine::createMovesForPiece(char piece_type) {
    std::pair<int,int> board_dims = {8, 8};
    
    // Map piece type to moves file
    std::string moves_path;
    switch(piece_type) {
        case 'P': moves_path = "server/moves/P.txt"; break;
        case 'R': moves_path = "server/moves/R.txt"; break;
        case 'N': moves_path = "server/moves/N.txt"; break;
        case 'B': moves_path = "server/moves/B.txt"; break;
        case 'Q': moves_path = "server/moves/Q.txt"; break;
        case 'K': moves_path = "server/moves/K.txt"; break;
        default:
            std::cout << "⚠️ Unknown piece type: " << piece_type << std::endl;
            moves_path = ""; // Empty path = no moves allowed
            break;
    }
    
    return std::make_shared<ServerMoves>(moves_path, board_dims);
}
#include "Game.hpp"
#include <opencv2/opencv.hpp>

// ---------------- Implementation --------------------
Game::Game(std::vector<PiecePtr> pcs, Board board)
    : pieces(pcs), board(board) {
    validate();
    for(const auto & p : pieces) piece_by_id[p->id] = p;
    start_tp = std::chrono::steady_clock::now();
}

int Game::game_time_ms() const {
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_tp).count());
}

Board Game::clone_board() const {
    return board.clone();
}

void Game::run(int num_iterations, bool is_with_graphics) {
    running_ = true;
    start_user_input_thread();
    int start_ms = game_time_ms();
    for(auto & p : pieces) p->reset(start_ms);

    run_game_loop(num_iterations, is_with_graphics);

    announce_win();
    
    // Close OpenCV windows if graphics were used
    if(is_with_graphics) {
        OpenCvImg::close_all_windows();
    }
    running_ = false;
}

void Game::start_user_input_thread() {
    running_ = true;
    std::cout << "[INPUT] Starting user input thread..." << std::endl;
    
    std::thread input_thread([this]() {
        while (running_) {
            int key = cv::waitKey(30) & 0xFF;
            if (key != 255) {
                std::cout << "[INPUT] Key pressed: " << key << std::endl;
                Command cmd(game_time_ms(), "", "", {});
                
                // Player 1 controls
                if (key == 82) cmd = Command(game_time_ms(), "", "up", {}, 1);
                else if (key == 84) cmd = Command(game_time_ms(), "", "down", {}, 1);
                else if (key == 81) cmd = Command(game_time_ms(), "", "left", {}, 1);
                else if (key == 83) cmd = Command(game_time_ms(), "", "right", {}, 1);
                else if (key == 13) cmd = Command(game_time_ms(), "", "select", {}, 1);
                else if (key == 32) cmd = Command(game_time_ms(), "", "switch", {}, 0); // Space to switch player
                
                // Player 2 controls
                else if (key == 'w') cmd = Command(game_time_ms(), "", "up", {}, 2);
                else if (key == 's') cmd = Command(game_time_ms(), "", "down", {}, 2);
                else if (key == 'a') cmd = Command(game_time_ms(), "", "left", {}, 2);
                else if (key == 'd') cmd = Command(game_time_ms(), "", "right", {}, 2);
                else if (key == 'f') cmd = Command(game_time_ms(), "", "select", {}, 2);
                
                if (!cmd.type.empty()) {
                    std::cout << "[INPUT] Adding command to queue: " << cmd.type << " (Player " << cmd.player_id << ")" << std::endl;
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    user_input_queue.push(cmd);
                    cv_.notify_one();
                } else {
                    std::cout << "[INPUT] Unknown key: " << key << std::endl;
                }
            }
        }
    });
    
    input_thread.detach();
}

void Game::run_game_loop(int num_iterations, bool is_with_graphics) {
    int it_counter = 0;
    std::cout << "Starting game loop with " << pieces.size() << " pieces..." << std::endl;
    std::cout << "Graphics enabled: " << (is_with_graphics ? "true" : "false") << std::endl;
    
    // Initialize all pieces first
    int now = game_time_ms();
    std::cout << "\n=== INITIALIZING ALL PIECES ===" << std::endl;
    for(size_t i = 0; i < pieces.size(); ++i) {
        auto& p = pieces[i];
        std::cout << "[" << (i+1) << "/" << pieces.size() << "] Initializing: " << p->id;
        
        try {
            p->update(now);
            
            if (p->state) {
                std::cout << " - INITIALIZED (keeping position)" << std::endl;
            } else {
                std::cout << " - WARNING: NO STATE!" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << " - ERROR: " << e.what() << std::endl;
        }
    }
    std::cout << "=== FINISHED INITIALIZING ALL PIECES ===\n" << std::endl;
    
    while(!is_win() && running_) {
        std::cout << "\n=== Iteration " << it_counter << " ===" << std::endl;
        now = game_time_ms();
        
        // Update all pieces
        for(auto & p : pieces) {
            p->update(now);
        }

        update_cell2piece_map();

        // Process user input with thread safety
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            // Process string commands from keyboard producers
            while(!string_input_queue.empty()) {
                auto cmd_str = string_input_queue.front();
                string_input_queue.pop();
                // Convert string command to Command object
                // For now, create a simple command - this may need enhancement
                Command cmd(now, "player1", cmd_str, {}, 1);
                user_input_queue.push(cmd);
            }
            
            while(!user_input_queue.empty()) {
                auto cmd = user_input_queue.front();
                user_input_queue.pop();
                process_input(cmd);
            }
        }

        if(is_with_graphics) {
            std::cout << "Drawing graphics..." << std::endl;
            // Create a copy of the board to draw pieces on
            auto display_board = board.clone();
            
            int pieces_drawn = 0;
            int pieces_failed = 0;
            
            // Draw all pieces on the board
            for(size_t i = 0; i < pieces.size(); ++i) {
                const auto& piece = pieces[i];
                
                auto cell = piece->current_cell();
                
                try {
                    if (!piece->state) {
                        pieces_failed++;
                        continue;
                    }
                    
                    if (!piece->state->graphics) {
                        pieces_failed++;
                        continue;
                    }
                    
                    // Update graphics before getting image
                    piece->state->graphics->update(now);
                    auto piece_img = piece->state->graphics->get_img();
                    if (!piece_img) {
                        pieces_failed++;
                        continue;
                    }
                    
                    auto pos_m = display_board.cell_to_m(cell);
                    auto pos_pix = display_board.m_to_pix(pos_m);
                    
                    // Count pieces at same cell for proper offset
                    int pieces_at_cell = 0;
                    for(size_t j = 0; j < pieces.size(); ++j) {
                        if(j < i && pieces[j]->current_cell() == cell) {
                            pieces_at_cell++;
                        }
                    }
                    
                    // Offset only horizontally for same row with 80 spacing
                    int offset_x = pieces_at_cell * 80;
                    int offset_y = 0;  // No vertical offset
                    
                    piece_img->draw_on(*display_board.img, pos_pix.first + offset_x, pos_pix.second + offset_y);
                    pieces_drawn++;
                    
                } catch (const std::exception& e) {
                    pieces_failed++;
                }
            }
            
            if (pieces_drawn > 0) {
                // Draw green border around current cursor position
                auto cursor_pos_m = display_board.cell_to_m(cursor_pos_);
                auto cursor_pos_pix = display_board.m_to_pix(cursor_pos_m);
                int cell_size = 80;
                display_board.img->draw_rect(cursor_pos_pix.first, cursor_pos_pix.second, 
                                           cell_size, cell_size, {0, 255, 0}); // Green border
                
                display_board.show();
                // Check for ESC key to exit
                int key = cv::waitKey(1);
                if(key == 27) { // ESC key
                    std::cout << "ESC pressed, exiting..." << std::endl;
                    return;
                }
            }
        }

        resolve_collisions();

        ++it_counter;
        // Run indefinitely unless ESC is pressed or win condition is met
        
        // Frame pacing - sleep for ~30ms to achieve ~30 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void Game::update_cell2piece_map() {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    pos.clear();
    for(const auto& p : pieces) {
        auto cell = p->current_cell();
        pos[cell].push_back(p);
    }
}

void Game::process_input(const Command& cmd) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    std::cout << "[PROCESS] Processing command: " << cmd.type << " from Player " << cmd.player_id << std::endl;
    
    if (cmd.type == "switch") {
        current_player_ = (current_player_ == 1) ? 2 : 1;
        std::cout << "[PROCESS] Switched to Player " << current_player_ << std::endl;
        return;
    }
    
    if (cmd.player_id != current_player_) {
        std::cout << "[PROCESS] Ignoring command from inactive player " << cmd.player_id << std::endl;
        return;
    }
    
    if (cmd.type == "up") move_cursor(0, -1);
    else if (cmd.type == "down") move_cursor(0, 1);
    else if (cmd.type == "left") move_cursor(-1, 0);
    else if (cmd.type == "right") move_cursor(1, 0);
    else if (cmd.type == "select") {
        auto cell_pieces_it = pos.find(cursor_pos_);
        if (cell_pieces_it != pos.end() && !cell_pieces_it->second.empty()) {
            selected_piece_ = cell_pieces_it->second[0];
            std::cout << "[PROCESS] Selected piece: " << selected_piece_->id << " at cursor position" << std::endl;
        }
    }
}

void Game::resolve_collisions() {
    check_captures();
}

void Game::announce_win() const {
    if(is_win()) {
        std::cout << "Game Over - Victory achieved!" << std::endl;
    } else {
        std::cout << "Game ended without victory condition." << std::endl;
    }
}

void Game::validate() {
    if(pieces.empty()) {
        throw InvalidBoard("No pieces provided");
    }
    
    if(board.W_cells <= 0 || board.H_cells <= 0) {
        throw InvalidBoard("Invalid board dimensions");
    }
}

bool Game::is_win() const {
    // Simple win condition - can be enhanced
    return false;
}

void Game::enqueue_command(const Command& cmd) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    user_input_queue.push(cmd);
    cv_.notify_one();
}

// Enhanced user interaction methods from CTD25_1
void Game::handle_mouse_click(int x, int y) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    if (!is_selecting_target_) {
        select_piece_at(x, y);
    } else {
        // Move selected piece to clicked position
        if (selected_piece_) {
            Command move_cmd(game_time_ms(), selected_piece_->id, "move", {{cursor_pos_.first, cursor_pos_.second}, {x, y}}, 1);
            enqueue_command(move_cmd);
        }
        cancel_selection();
    }
}

void Game::handle_key_press(int key) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    switch(key) {
        case 27: // ESC
            cancel_selection();
            break;
        case 13: // Enter
            confirm_move();
            break;
        // Arrow keys for cursor movement
        case 'w': case 'W':
            move_cursor(0, -1);
            break;
        case 's': case 'S':
            move_cursor(0, 1);
            break;
        case 'a': case 'A':
            move_cursor(-1, 0);
            break;
        case 'd': case 'D':
            move_cursor(1, 0);
            break;
    }
}

void Game::select_piece_at(int x, int y) {
    auto cell_pieces_it = pos.find({x, y});
    if (cell_pieces_it != pos.end() && !cell_pieces_it->second.empty()) {
        selected_piece_ = cell_pieces_it->second[0];
        cursor_pos_ = {x, y};
        is_selecting_target_ = true;
        std::cout << "Selected piece: " << selected_piece_->id << " at " << cell_to_chess_notation(x, y) << std::endl;
    }
}

void Game::move_cursor(int dx, int dy) {
    cursor_pos_.first = std::max(0, std::min(board.W_cells - 1, cursor_pos_.first + dx));
    cursor_pos_.second = std::max(0, std::min(board.H_cells - 1, cursor_pos_.second + dy));
    std::cout << "Cursor moved to: (" << cursor_pos_.first << "," << cursor_pos_.second << ")" << std::endl;
}

void Game::confirm_move() {
    if (selected_piece_ && is_selecting_target_) {
        auto start_cell = selected_piece_->current_cell();
        Command move_cmd(game_time_ms(), selected_piece_->id, "move", {start_cell, cursor_pos_}, 1);
        enqueue_command(move_cmd);
        std::cout << "Move confirmed: " << selected_piece_->id << " to " << cell_to_chess_notation(cursor_pos_.first, cursor_pos_.second) << std::endl;
    }
    cancel_selection();
}

void Game::cancel_selection() {
    selected_piece_ = nullptr;
    is_selecting_target_ = false;
    std::cout << "Selection cancelled" << std::endl;
}

std::string Game::cell_to_chess_notation(int x, int y) {
    return std::string(1, 'a' + y) + std::to_string(x + 1);
}

PiecePtr Game::find_piece_by_id(const std::string& id) {
    auto it = piece_by_id.find(id);
    return (it != piece_by_id.end()) ? it->second : nullptr;
}

void Game::check_captures() {
    // Enhanced capture logic
    for (const auto& [cell, pieces_at_cell] : pos) {
        if (pieces_at_cell.size() > 1) {
            // Multiple pieces at same cell - check for captures
            for (size_t i = 0; i < pieces_at_cell.size(); ++i) {
                for (size_t j = i + 1; j < pieces_at_cell.size(); ++j) {
                    auto piece1 = pieces_at_cell[i];
                    auto piece2 = pieces_at_cell[j];
                    
                    if (piece1->state->can_capture() && piece2->state->can_be_captured()) {
                        capture_piece(piece2, piece1);
                    } else if (piece2->state->can_capture() && piece1->state->can_be_captured()) {
                        capture_piece(piece1, piece2);
                    }
                }
            }
        }
    }
}

void Game::capture_piece(PiecePtr captured, PiecePtr captor) {
    std::cout << "Piece captured: " << captured->id << " by " << captor->id << std::endl;
    
    // Remove captured piece from pieces vector
    pieces.erase(std::remove(pieces.begin(), pieces.end(), captured), pieces.end());
    
    // Remove from piece_by_id map
    piece_by_id.erase(captured->id);
}

std::string Game::get_position_key(int x, int y) {
    return std::to_string(x) + "," + std::to_string(y);
}

// Factory function implementation
Game create_game(const std::string& pieces_root, ImgFactoryPtr img_factory) {
    std::cout << "Creating game from pieces root: " << pieces_root << std::endl;
    
    // Load board image
    std::string board_img_path = pieces_root + "board.png";
    auto board_img = img_factory->load(board_img_path, {640, 640});
    if (!board_img) {
        throw std::runtime_error("Failed to load board image: " + board_img_path);
    }
    
    // Create board (8x8 chess board)
    Board board(80, 80, 8, 8, board_img);
    
    // Create graphics factory
    GraphicsFactory gfx_factory(img_factory);
    
    // Create piece factory
    PieceFactory piece_factory(board, pieces_root, gfx_factory);
    
    // Load pieces from board.csv
    std::string board_csv_path = pieces_root + "board.csv";
    auto pieces = piece_factory.create_pieces_from_board_csv(board_csv_path);
    
    std::cout << "Created " << pieces.size() << " pieces" << std::endl;
    
    return Game(pieces, board);
}
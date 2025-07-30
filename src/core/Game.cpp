#include "Game.hpp"
#include "../game_logic/CaptureRules.hpp"
#include <opencv2/opencv.hpp>
#include <set>
#include <cstdio>
#include "Physics.hpp"
#include "../observer/headers/MoveEvent.hpp"
#include "../observer/headers/CaptureEvent.hpp"
#include "../observer/headers/GameStateEvent.hpp"

// ---------------- Implementation --------------------
Game::Game(std::vector<PiecePtr> pcs, Board board)
    : pieces(pcs), board(board), background_img_(nullptr),
    // Player 1 (White) starts at bottom-right
    cursor_pos_player1_({board.W_cells - 1, board.H_cells - 1}),
    selected_piece_pos_player1_({-1, -1}),
    is_selecting_target_player1_(false),
    // Player 2 (Black) starts at top-left
    cursor_pos_player2_({0, 0}),
    selected_piece_pos_player2_({-1, -1}),
    is_selecting_target_player2_(false) {
    validate();
    for(const auto & p : pieces) {
        if (p) {
            piece_by_id[p->id] = p;
        }
    }
    start_tp = std::chrono::steady_clock::now();
    // Initialize position map
    update_cell2piece_map();
    // Setup event observers
    setupEventListeners();
    
    // Setup key mappings for Player 1 (Arrow keys + Enter)
    keymap_player1_ = {
        {2490368, "left"},   // Left arrow
        {2555904, "down"},   // Down arrow
        {2424832, "up"},     // Up arrow
        {2621440, "right"},  // Right arrow
        {13, "select"},      // Enter
        {27, "exit"}         // ESC
    };
    
    // Setup key mappings for Player 2 (WASD + Space)
    keymap_player2_ = {
        {65, "left"},    // 'A'
        {83, "down"},    // 'S'
        {87, "up"},      // 'W'
        {68, "right"},   // 'D'
        {32, "select"},  // Space
        {27, "exit"}     // ESC
    };
}

Game::Game(std::vector<PiecePtr> pcs, Board board, ImgPtr background_img)
    : pieces(pcs), board(board), background_img_(background_img),
    // Player 1 (White) starts at bottom-right
    cursor_pos_player1_({board.W_cells - 1, board.H_cells - 1}),
    selected_piece_pos_player1_({-1, -1}),
    is_selecting_target_player1_(false),
    // Player 2 (Black) starts at top-left
    cursor_pos_player2_({0, 0}),
    selected_piece_pos_player2_({-1, -1}),
    is_selecting_target_player2_(false) {
    validate();
    for(const auto & p : pieces) {
        if (p) {
            piece_by_id[p->id] = p;
        }
    }
    start_tp = std::chrono::steady_clock::now();
    // Initialize position map
    update_cell2piece_map();
    // Setup event observers
    setupEventListeners();
    
    // Setup key mappings for Player 1 (Arrow keys + Enter)
    keymap_player1_ = {
        {2490368, "left"},   // Left arrow
        {2555904, "down"},   // Down arrow
        {2424832, "up"},     // Up arrow
        {2621440, "right"},  // Right arrow
        {13, "select"},      // Enter
        {27, "exit"}         // ESC
    };
    
    // Setup key mappings for Player 2 (WASD + Space)
    keymap_player2_ = {
        {65, "left"},    // 'A'
        {83, "down"},    // 'S'
        {87, "up"},      // 'W'
        {68, "right"},   // 'D'
        {32, "select"},  // Space
        {27, "exit"}     // ESC
    };
}

int Game::game_time_ms() const {
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_tp).count());
}

void Game::setNetworkInterface(NetworkInterface* network) {
    network_interface_ = network;
}

Board Game::clone_board() const {
    return board.clone();
}

void Game::run(int num_iterations, bool is_with_graphics) {
    running_ = true;
    
    // פרסום תחילת משחק
    GameStateEvent startEvent(GameState::Playing, GameState::Paused, "Game started", game_time_ms());
    eventManager_.publish(startEvent, "gamestate");
    currentGameState_ = GameState::Playing;
    
    // Handle game state change for message display
    onGameStateChanged(startEvent);
    
    // הצגת הודעת פתיחה למשך 3 שניות לפני תחילת המשחק
    if (is_with_graphics) {
        int opening_start = game_time_ms();
        while (game_time_ms() - opening_start < 3000) { // 3 שניות
            // Update message display
            updateMessageDisplay();
            
            // Display opening message
            if (background_img_) {
                auto display_img = background_img_->clone();
                drawGameInterface(display_img);
                
                // Draw board in center
                Board display_board = board.clone();
                int board_x_offset = (1280 - 640) / 2;
                int board_y_offset = (960 - 640) / 2;
                display_board.img->draw_on(*display_img, board_x_offset, board_y_offset);
                
                // Draw all pieces in starting positions
                for(const auto& piece : pieces) {
                    if (piece && piece->state && piece->state->graphics) {
                        piece->state->graphics->update(game_time_ms());
                        auto piece_img = piece->state->graphics->get_img();
                        if (piece_img) {
                            auto cell = piece->current_cell();
                            auto pos_m = display_board.cell_to_m(cell);
                            auto pos_pix = display_board.m_to_pix(pos_m);
                            pos_pix.first += board_x_offset;
                            pos_pix.second += board_y_offset;
                            piece_img->draw_on(*display_img, pos_pix.first, pos_pix.second);
                        }
                    }
                }
                
                // Draw message AFTER board and pieces (so it appears on top)
                drawMessage(display_img);
                
                display_img->show();
            }
            
            // Check for ESC to abort
            int key = cv::waitKeyEx(30);
            if (key == 27) { // ESC
                running_ = false;
                return;
            }
        }
    }
    
    start_user_input_thread();
    int start_ms = game_time_ms();
    // Don't call reset - it breaks piece positions
    // for(auto & p : pieces) p->reset(start_ms);

    run_game_loop(num_iterations, is_with_graphics);

    announce_win();
    
    // הצגת הודעת סיום למשך 2 שניות לפני סגירת החלון
    if (is_with_graphics && is_win()) {
        int ending_start = game_time_ms();
        while (game_time_ms() - ending_start < 2000) { // 2 שניות
            // Update message display
            updateMessageDisplay();
            
            // Display ending message with final board state
            if (background_img_) {
                auto display_img = background_img_->clone();
                drawGameInterface(display_img);
                
                // Draw board in center
                Board display_board = board.clone();
                int board_x_offset = (1280 - 640) / 2;
                int board_y_offset = (960 - 640) / 2;
                display_board.img->draw_on(*display_img, board_x_offset, board_y_offset);
                
                // Draw remaining pieces in final positions
                for(const auto& piece : pieces) {
                    if (piece && piece->state && piece->state->graphics) {
                        piece->state->graphics->update(game_time_ms());
                        auto piece_img = piece->state->graphics->get_img();
                        if (piece_img) {
                            auto cell = piece->current_cell();
                            auto pos_m = display_board.cell_to_m(cell);
                            auto pos_pix = display_board.m_to_pix(pos_m);
                            pos_pix.first += board_x_offset;
                            pos_pix.second += board_y_offset;
                            piece_img->draw_on(*display_img, pos_pix.first, pos_pix.second);
                        }
                    }
                }
                
                // Draw message AFTER board and pieces (so it appears on top)
                drawMessage(display_img);
                
                display_img->show();
            }
            
            // Allow user to press ESC to close early
            int key = cv::waitKeyEx(30);
            if (key == 27) { // ESC
                break;
            }
        }
    }
    
    // Close OpenCV windows if graphics were used
    if(is_with_graphics) {
        OpenCvImg::close_all_windows();
    }
    running_ = false;
}

void Game::start_user_input_thread() {
    running_ = true;
}

void Game::run_game_loop(int num_iterations, bool is_with_graphics) {
    int it_counter = 0;
    // Initialize all pieces first
    int now = game_time_ms();
    for(size_t i = 0; i < pieces.size(); ++i) {
        auto& p = pieces[i];
        try {
            p->update(now);
        } catch (const std::exception& e) {
            // Silent error handling
        }
    }
    
    while(!is_win() && running_) {
        now = game_time_ms();
        
        // Update message display timing
        updateMessageDisplay();
        
        // Update all pieces
        for(auto & p : pieces) {
            p->update(now);
        }

        update_cell2piece_map();

        // Input processing moved to graphics section where OpenCV handles keys

        if(is_with_graphics) {
            // Use background image if available, otherwise create board copy
            ImgPtr display_img;
            Board display_board = board.clone();
            
            if (background_img_) {
                // Clone background and draw interface
                display_img = background_img_->clone();
                drawGameInterface(display_img);
                
                // Calculate board position on background (centered) - עדכנתי לגודל החדש
                int board_x_offset = (1280 - 640) / 2;  // 320 instead of 192
                int board_y_offset = (960 - 640) / 2;   // 160 instead of 64
                
                // Draw board on background
                display_board.img->draw_on(*display_img, board_x_offset, board_y_offset);
                
                int pieces_drawn = 0;
                int pieces_failed = 0;
                
                // Draw all pieces on the board (with offset for background)
                for(const auto& piece : pieces) {
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

                        // Use physics position for moving pieces, cell position for static pieces
                        std::pair<int, int> pos_pix;
                        if (piece->state->name == "move" || piece->state->name == "jump") {
                            auto pos_m = piece->state->physics->get_pos_m();
                            pos_pix = piece->state->physics->get_pos_pix();
                            // Fallback: if position is (0,0), use cell position instead
                            if (pos_m.first == 0.0 && pos_m.second == 0.0) {
                                auto pos_m_fallback = display_board.cell_to_m(cell);
                                pos_pix = display_board.m_to_pix(pos_m_fallback);
                            }
                        } else {
                            auto pos_m = display_board.cell_to_m(cell);
                            pos_pix = display_board.m_to_pix(pos_m);
                        }

                        // Add offset for background positioning
                        pos_pix.first += board_x_offset;
                        pos_pix.second += board_y_offset;
                        
                        piece_img->draw_on(*display_img, pos_pix.first, pos_pix.second);
                        pieces_drawn++;
                        
                    } catch (const std::exception& e) {
                        pieces_failed++;
                    }
                }
                
                if (pieces_drawn > 0) {
                    int cell_size = 80;
                    
                    // Draw Player 1 cursor (White - Green border)
                    auto cursor1_pos_m = display_board.cell_to_m(cursor_pos_player1_);
                    auto cursor1_pos_pix = display_board.m_to_pix(cursor1_pos_m);
                    cursor1_pos_pix.first += board_x_offset;
                    cursor1_pos_pix.second += board_y_offset;
                    display_img->draw_rect(cursor1_pos_pix.first, cursor1_pos_pix.second, 
                                          cell_size, cell_size, {0, 255, 0}); // Green for Player 1
                    
                    // Draw Player 2 cursor (Black - Red border)
                    auto cursor2_pos_m = display_board.cell_to_m(cursor_pos_player2_);
                    auto cursor2_pos_pix = display_board.m_to_pix(cursor2_pos_m);
                    cursor2_pos_pix.first += board_x_offset;
                    cursor2_pos_pix.second += board_y_offset;
                    display_img->draw_rect(cursor2_pos_pix.first, cursor2_pos_pix.second, 
                                          cell_size, cell_size, {255, 0, 0}); // Red for Player 2
                    
                    // Draw Player 1 selected piece (Blue border)
                    if (selected_piece_player1_) {
                        auto selected1_pos_m = display_board.cell_to_m(selected_piece_pos_player1_);
                        auto selected1_pos_pix = display_board.m_to_pix(selected1_pos_m);
                        selected1_pos_pix.first += board_x_offset;
                        selected1_pos_pix.second += board_y_offset;
                        display_img->draw_rect(selected1_pos_pix.first, selected1_pos_pix.second, 
                                              cell_size, cell_size, {0, 0, 255}); // Blue for selected P1
                    }
                    
                    // Draw Player 2 selected piece (Yellow border)
                    if (selected_piece_player2_) {
                        auto selected2_pos_m = display_board.cell_to_m(selected_piece_pos_player2_);
                        auto selected2_pos_pix = display_board.m_to_pix(selected2_pos_m);
                        selected2_pos_pix.first += board_x_offset;
                        selected2_pos_pix.second += board_y_offset;
                        display_img->draw_rect(selected2_pos_pix.first, selected2_pos_pix.second, 
                                              cell_size, cell_size, {0, 255, 255}); // Yellow for selected P2
                    }
                    
                    display_img->show();
                }
            } else {
                // Fallback to original board-only display
                int pieces_drawn = 0;
                int pieces_failed = 0;
                
                // Draw all pieces on the board
                for(const auto& piece : pieces) {
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

                        // Use physics position for moving pieces, cell position for static pieces
                        std::pair<int, int> pos_pix;
                        if (piece->state->name == "move" || piece->state->name == "jump") {
                            auto pos_m = piece->state->physics->get_pos_m();
                            pos_pix = piece->state->physics->get_pos_pix();
                            // Fallback: if position is (0,0), use cell position instead
                            if (pos_m.first == 0.0 && pos_m.second == 0.0) {
                                auto pos_m_fallback = display_board.cell_to_m(cell);
                                pos_pix = display_board.m_to_pix(pos_m_fallback);
                            }
                        } else {
                            auto pos_m = display_board.cell_to_m(cell);
                            pos_pix = display_board.m_to_pix(pos_m);
                        }

                        piece_img->draw_on(*display_board.img, pos_pix.first, pos_pix.second);
                        pieces_drawn++;
                        
                    } catch (const std::exception& e) {
                        pieces_failed++;
                    }
                }
                
                if (pieces_drawn > 0) {
                    int cell_size = 80;
                    
                    // Draw Player 1 cursor (White - Green border)
                    auto cursor1_pos_m = display_board.cell_to_m(cursor_pos_player1_);
                    auto cursor1_pos_pix = display_board.m_to_pix(cursor1_pos_m);
                    display_board.img->draw_rect(cursor1_pos_pix.first, cursor1_pos_pix.second, 
                                                cell_size, cell_size, {0, 255, 0}); // Green for Player 1
                    
                    // Draw Player 2 cursor (Black - Red border)
                    auto cursor2_pos_m = display_board.cell_to_m(cursor_pos_player2_);
                    auto cursor2_pos_pix = display_board.m_to_pix(cursor2_pos_m);
                    display_board.img->draw_rect(cursor2_pos_pix.first, cursor2_pos_pix.second, 
                                                cell_size, cell_size, {255, 0, 0}); // Red for Player 2
                    
                    // Draw Player 1 selected piece (Blue border)
                    if (selected_piece_player1_) {
                        auto selected1_pos_m = display_board.cell_to_m(selected_piece_pos_player1_);
                        auto selected1_pos_pix = display_board.m_to_pix(selected1_pos_m);
                        display_board.img->draw_rect(selected1_pos_pix.first, selected1_pos_pix.second, 
                                                   cell_size, cell_size, {0, 0, 255}); // Blue for selected P1
                    }
                    
                    // Draw Player 2 selected piece (Yellow border)
                    if (selected_piece_player2_) {
                        auto selected2_pos_m = display_board.cell_to_m(selected_piece_pos_player2_);
                        auto selected2_pos_pix = display_board.m_to_pix(selected2_pos_m);
                        display_board.img->draw_rect(selected2_pos_pix.first, selected2_pos_pix.second, 
                                                   cell_size, cell_size, {0, 255, 255}); // Yellow for selected P2
                    }
                    
                    display_board.show();
                }
            }
            
            // Handle input in main loop where window exists
            int key = cv::waitKeyEx(30);
            if (key != -1) {
                // Player 1 controls (Arrow keys + Enter)
                if (keymap_player1_.find(key) != keymap_player1_.end()) {
                    process_input(1, keymap_player1_[key]);
                }
                // Player 2 controls (WASD + Space)
                else if (keymap_player2_.find(key) != keymap_player2_.end()) {
                    process_input(2, keymap_player2_[key]);
                }
                // ESC exits for both players
                else if (key == 27) {
                    return;
                }
            }
        }

        resolve_collisions();

        ++it_counter;
        // Run indefinitely unless ESC is pressed or win condition is met
        
        // Frame pacing handled by cv::waitKey(30)
    }
}

void Game::update_cell2piece_map() {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    pos.clear();
    for(const auto& p : pieces) {
        if (p && p->state && p->state->physics) {
            auto cell = p->current_cell();
            // Validate cell coordinates
            if (cell.first >= 0 && cell.first < board.W_cells && 
                cell.second >= 0 && cell.second < board.H_cells) {
                pos[cell].push_back(p);
            }
        }
    }
}

void Game::process_input(int player_id, const std::string& cmd_type) {
    // אם יש לנו network interface, שולחים את הקלט לשרת במקום לעבד מקומית
    if (network_interface_) {
        std::string input_message = "INPUT:" + std::to_string(player_id) + ":" + cmd_type;
        network_interface_->sendMove(input_message);
        return;
    }
    
    // אם אין network interface, מעבדים מקומית (למשחק מקומי)
    process_input_local(player_id, cmd_type);
}

void Game::process_input_local(int player_id, const std::string& cmd_type) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    
    // Get player-specific state
    PiecePtr& selected_piece = (player_id == 1) ? selected_piece_player1_ : selected_piece_player2_;
    std::pair<int, int>& cursor_pos = (player_id == 1) ? cursor_pos_player1_ : cursor_pos_player2_;
    std::pair<int, int>& selected_piece_pos = (player_id == 1) ? selected_piece_pos_player1_ : selected_piece_pos_player2_;
    bool& is_selecting_target = (player_id == 1) ? is_selecting_target_player1_ : is_selecting_target_player2_;
    
    if (cmd_type == "up") move_cursor(player_id, 0, -1);
    else if (cmd_type == "down") move_cursor(player_id, 0, 1);
    else if (cmd_type == "left") move_cursor(player_id, -1, 0);
    else if (cmd_type == "right") move_cursor(player_id, 1, 0);
    else if (cmd_type == "select") {
        // Update position map before accessing it
        update_cell2piece_map();
        
        // Enhanced selection logic from movement-logic branch
        if (selected_piece == nullptr) {
            // First press - pick up piece under cursor
            auto cell_pieces_it = pos.find(cursor_pos);
            if (cell_pieces_it != pos.end() && !cell_pieces_it->second.empty()) {
                auto piece = cell_pieces_it->second[0];
                // Check if player can control this piece
                if (piece && piece->id.length() >= 2) {
                    char piece_color = piece->id[1];
                    bool can_control = (player_id == 1 && piece_color == 'W') || (player_id == 2 && piece_color == 'B');
                    if (can_control) {
                        selected_piece = piece;
                        selected_piece_pos = cursor_pos;
                    }
                }
            }
        } else if (cursor_pos == selected_piece_pos) {
            // Same position - deselect
            selected_piece = nullptr;
            selected_piece_pos = {-1, -1};
        } else {
            // Different position - validate and create move command
            if (is_move_valid(selected_piece, selected_piece_pos, cursor_pos)) {
                try {
                    std::vector<std::pair<int,int>> move_params = {selected_piece_pos, cursor_pos};
                    Command move_cmd(game_time_ms(), selected_piece->id, "move", move_params);
                    
                    // Process the move command through state machine
                    auto piece_it = piece_by_id.find(move_cmd.piece_id);
                    if (piece_it != piece_by_id.end()) {
                        auto piece = piece_it->second;
                        if (piece && piece->state) {
                            // Update position map again before passing to piece
                            update_cell2piece_map();
                            piece->on_command(move_cmd, pos);
                            
                            // פרסום אירוע תזוזה
                            bool isWhite = (piece->id.length() >= 2 && piece->id[1] == 'W');
                            MoveEvent moveEvent(
                                piece->id, 
                                cellToChessNotation(selected_piece_pos.first, selected_piece_pos.second),
                                cellToChessNotation(cursor_pos.first, cursor_pos.second),
                                isWhite,
                                game_time_ms()
                            );
                            eventManager_.publish(moveEvent, "move");
                        }
                    }
                } catch (const std::exception& e) {
                    // Handle move command errors silently
                }
            }
            
            // Reset selection
            selected_piece = nullptr;
            selected_piece_pos = {-1, -1};
        }
    }
}

void Game::move_cursor(int player_id, int dx, int dy) {
    std::pair<int, int>& cursor_pos = (player_id == 1) ? cursor_pos_player1_ : cursor_pos_player2_;
    cursor_pos.first = (std::max)(0, (std::min)(board.W_cells - 1, cursor_pos.first + dx));
    cursor_pos.second = (std::max)(0, (std::min)(board.H_cells - 1, cursor_pos.second + dy));
}

void Game::resolve_collisions() {
    check_captures();
}

void Game::announce_win() {
    // בדיקת מצב הזכייה ופרסום אירוע
    if (is_win()) {
        // ספירת מלכים לקביעת מנצח
        bool whiteKingAlive = false;
        bool blackKingAlive = false;
        
        for (const auto& piece : pieces) {
            if (piece->id.length() >= 2 && piece->id[0] == 'K') {
                if (piece->id[1] == 'W') whiteKingAlive = true;
                else if (piece->id[1] == 'B') blackKingAlive = true;
            }
        }
        
        GameState winState;
        std::string reason;
        if (whiteKingAlive && !blackKingAlive) {
            winState = GameState::WhiteWin;
            reason = "Black king captured - White wins!";
        } else if (blackKingAlive && !whiteKingAlive) {
            winState = GameState::BlackWin;  
            reason = "White king captured - Black wins!";
        } else {
            winState = GameState::Draw;
            reason = "Both kings eliminated - Draw!";
        }
        
        // פרסום אירוע זכייה
        GameStateEvent winEvent(winState, currentGameState_, reason, game_time_ms());
        eventManager_.publish(winEvent, "gamestate");
        currentGameState_ = winState;
        
        // Handle game state change for message display
        onGameStateChanged(winEvent);
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
    int king_count = 0;

    for (const auto& piece : pieces) {
        if (piece->id.at(0)=='K') {
            king_count++;
        }
    }
    return king_count < 2;
}


void Game::enqueue_command(const Command& cmd) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    user_input_queue.push(cmd);
    cv_.notify_one();
}

void Game::handle_mouse_click(int x, int y) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    // Mouse click defaults to Player 1
    if (!is_selecting_target_player1_) {
        select_piece_at(x, y);
    } else {
        if (selected_piece_player1_) {
            auto start_cell = selected_piece_player1_->current_cell();
            if (is_move_valid(selected_piece_player1_, start_cell, {x, y})) {
                std::vector<std::pair<int,int>> move_params = {start_cell, {x, y}};
                Command move_cmd(game_time_ms(), selected_piece_player1_->id, "move", move_params, 1);
                
                // Send move to network if connected
                if (network_interface_) {
                    std::string move_str = std::to_string(start_cell.first) + "," + 
                                         std::to_string(start_cell.second) + ":" + 
                                         std::to_string(x) + "," + std::to_string(y) + 
                                         ":" + selected_piece_player1_->id;
                    network_interface_->sendMove(move_str);
                    std::cout << "📤 Sent move to network: " << move_str << std::endl;
                }
                
                enqueue_command(move_cmd);
            }
        }
        cancel_selection();
    }
}

void Game::handle_key_press(int key) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    // Player 1 controls (Arrow keys + Enter)
    if (keymap_player1_.find(key) != keymap_player1_.end()) {
        process_input(1, keymap_player1_[key]);
    }
    // Player 2 controls (WASD + Space)
    else if (keymap_player2_.find(key) != keymap_player2_.end()) {
        process_input(2, keymap_player2_[key]);
    }
}

void Game::select_piece_at(int x, int y) {
    auto cell_pieces_it = pos.find({x, y});
    if (cell_pieces_it != pos.end() && !cell_pieces_it->second.empty()) {
        selected_piece_player1_ = cell_pieces_it->second[0];
        cursor_pos_player1_ = {x, y};
        is_selecting_target_player1_ = true;
    }
}

void Game::confirm_move() {
    if (selected_piece_player1_ && is_selecting_target_player1_) {
        auto start_cell = selected_piece_player1_->current_cell();
        if (is_move_valid(selected_piece_player1_, start_cell, cursor_pos_player1_)) {
            std::vector<std::pair<int,int>> move_params = {start_cell, cursor_pos_player1_};
            Command move_cmd(game_time_ms(), selected_piece_player1_->id, "move", move_params, 1);
            enqueue_command(move_cmd);
            
            // Broadcast move to network
            std::string network_move = selected_piece_player1_->id + ":" + 
                                     std::to_string(start_cell.first) + "," + std::to_string(start_cell.second) + ":" +
                                     std::to_string(cursor_pos_player1_.first) + "," + std::to_string(cursor_pos_player1_.second);
            broadcastMove(network_move);
        }
    }
    cancel_selection();
}

void Game::cancel_selection() {
    selected_piece_player1_ = nullptr;
    selected_piece_player2_ = nullptr;
    is_selecting_target_player1_ = false;
    is_selecting_target_player2_ = false;
}

std::string Game::cell_to_chess_notation(int x, int y) {
    return std::string(1, 'a' + y) + std::to_string(x + 1);
}

PiecePtr Game::find_piece_by_id(const std::string& id) {
    auto it = piece_by_id.find(id);
    return (it != piece_by_id.end()) ? it->second : nullptr;
}

void Game::check_captures() {
    // Create a copy of the position map to avoid iterator invalidation
    auto pos_copy = pos;
    
    static std::set<std::string> already_captured; // Keep track globally
    static std::set<std::pair<std::string, std::string>> reported_collisions; // Track reported collisions
    
    for (const auto& [cell, pieces_at_cell] : pos_copy) {
        if (pieces_at_cell.size() > 1) {
            CaptureRules::print_collision_summary(cell, pieces_at_cell);
            
            // בדוק כל זוג חתיכות
            for (size_t i = 0; i < pieces_at_cell.size(); ++i) {
                for (size_t j = i + 1; j < pieces_at_cell.size(); ++j) {
                    auto piece1 = pieces_at_cell[i];
                    auto piece2 = pieces_at_cell[j];
                    
                    // Validate pieces before processing
                    if (piece1 && piece2 && piece1->state && piece2->state && 
                        piece1->id.length() >= 2 && piece2->id.length() >= 2) {
                        
                        // Enhanced capture callback with Knight logic
                        auto capture_callback = [this, cell](PiecePtr captured, PiecePtr captor) {
                            // Skip capture if knight is not at its target destination
                            if (captor->id.size() > 0 && captor->id[0] == 'N' && 
                                captor->state->physics->end_cell != cell) {
                                return; // Knight doesn't capture unless at target
                            }
                            this->capture_piece(captured, captor);
                        };
                        
                        if (CaptureRules::process_collision_pair(piece1, piece2, already_captured, 
                                                                     reported_collisions, game_time_ms(), 
                                                                     capture_callback)) {
                            return; // Exit after first capture
                        }
                    }
                }
            }
        }
    }
}

void Game::capture_piece(PiecePtr captured, PiecePtr captor) {
    std::cout << "capture_piece CALLED: " << (captured ? captured->id : "NULL") 
              << " captured by " << (captor ? captor->id : "NULL") << std::endl;
    
    if (captured && captor) {
        // פרסום אירוע תפיסה לפני ביצוע התפיסה
        bool capturerIsWhite = (captor->id.length() >= 2 && captor->id[1] == 'W');
        auto capturedCell = captured->current_cell();
        
        // הסרתי את הודעת התפיסה על המסך - רק הודעות תחילה וסיום
        
        // חישוב ערך הכלי שנתפס
        int pieceValue = 1; // ערך בסיסי
        
        if (captured->id.length() >= 1) {
            char pieceType = captured->id[0];
            
            switch(pieceType) {
                case 'P': pieceValue = 1; break;  // Pawn
                case 'R': pieceValue = 5; break;  // Rook
                case 'N': pieceValue = 3; break;  // Knight
                case 'B': pieceValue = 3; break;  // Bishop
                case 'Q': pieceValue = 9; break;  // Queen
                case 'K': pieceValue = 100; break; // King
                default: 
                    pieceValue = 1; // Default value
                    break;
            }
        }
        
        CaptureEvent captureEvent(
            captor->id,
            captured->id,
            cellToChessNotation(capturedCell.first, capturedCell.second),
            capturerIsWhite,
            pieceValue,
            game_time_ms()
        );
        eventManager_.publish(captureEvent, "capture");
        
        // ביצוע התפיסה
        pieces.erase(std::remove(pieces.begin(), pieces.end(), captured), pieces.end());
        piece_by_id.erase(captured->id);
        // Update position map after capture
        update_cell2piece_map();
    }
}

std::string Game::get_position_key(int x, int y) {
    return std::to_string(x) + "," + std::to_string(y);
}

bool Game::is_move_valid(PiecePtr piece, const std::pair<int,int>& from, const std::pair<int,int>& to) {
    if (!piece || !piece->state || !piece->state->moves) {
        std::cout << "MOVE_VALIDATION: Invalid piece or state" << std::endl;
        return false;
    }
    
    std::cout << "MOVE_VALIDATION: " << piece->id << " מנסה לזוז מ-(" << from.first << "," << from.second 
              << ") ל-(" << to.first << "," << to.second << ")" << std::endl;
    
    // Check if piece can move (not in rest state)
    if (piece->state->name == "long_rest" || piece->state->name == "short_rest") {
        return false;
    }
    
    // Check if piece is currently moving
    if (piece->state->name == "move" || piece->state->name == "jump") {
        return false;
    }
    
    // Check bounds
    if (to.first < 0 || to.first >= board.H_cells || 
        to.second < 0 || to.second >= board.W_cells) {
        return false;
    }
    
    // Check if trying to move to same position
    if (from == to) {
        return false;
    }
    
    // Verify piece is actually at the 'from' position
    auto current_cell = piece->current_cell();
    if (current_cell != from) {
        return false;
    }
    
    // Update position map to ensure accuracy
    update_cell2piece_map();
    
    // Check if destination has piece of same color
    auto dest_it = pos.find(to);
    if (dest_it != pos.end() && !dest_it->second.empty()) {
        auto dest_piece = dest_it->second[0];
        if (are_same_color(piece, dest_piece)) {
            return false;
        }
    }
    
    // Create set of occupied cells for path checking
    std::unordered_set<std::pair<int,int>, PairHash> occupied_cells;
    for (const auto& [cell, pieces_at_cell] : pos) {
        if (!pieces_at_cell.empty()) {
            occupied_cells.insert(cell);
            // std::cout << "OCCUPIED CELL: (" << cell.first << "," << cell.second << ") בה נמצא " << pieces_at_cell[0]->id << std::endl;
        }
    }
    
    // Check if target cell has pieces and what team they are
    auto target_pieces_it = pos.find(to);
    if (target_pieces_it != pos.end() && !target_pieces_it->second.empty()) {
        auto target_piece = target_pieces_it->second[0];
        if (target_piece && target_piece->id.length() >= 2 && piece->id.length() >= 2) {
            char moving_team = piece->id[1];
            char target_team = target_piece->id[1];
            std::cout << "MOVE_VALIDATION: " << piece->id << " (team " << moving_team 
                      << ") wants to move to cell with " << target_piece->id << " (team " << target_team << ")" << std::endl;
            
            if (moving_team == target_team) {
                std::cout << "MOVE_VALIDATION: BLOCKED - Same team!" << std::endl;
                return false; // Block same-team moves
            }
        }
    }
    
    std::cout << "CALLING piece->state->moves->is_valid() עם " << occupied_cells.size() << " תאים תפוסים" << std::endl;
    bool result = piece->state->moves->is_valid(from, to, occupied_cells);
    std::cout << "MOVE_VALIDATION: Result = " << (result ? "VALID" : "INVALID") << std::endl;
    
    if (!result) {
        std::cout << "MOVE FAILED ANALYSIS:" << std::endl;
        std::cout << "  Piece type: " << piece->id[0] << std::endl;
        std::cout << "  Distance: dx=" << abs(to.first - from.first) << ", dy=" << abs(to.second - from.second) << std::endl;
        std::cout << "  Path blocking check needed..." << std::endl;
    }
    
    return result;
}

char Game::get_piece_color(PiecePtr piece) {
    if (!piece || piece->id.size() < 2) {
        return '?';
    }
    // Color is the second character (after piece type): PW, PB, RW, etc.
    return piece->id[1]; // Second character is color (W/B)
}

bool Game::are_same_color(PiecePtr piece1, PiecePtr piece2) {
    if (!piece1 || !piece2) {
        return false;
    }
    char color1 = get_piece_color(piece1);
    char color2 = get_piece_color(piece2);
    return (color1 == color2);
}

void Game::setupEventListeners() {
    // רישום מאזינים לתזוזות
    eventManager_.subscribe(&whiteMovesTracker_, "move");
    eventManager_.subscribe(&blackMovesTracker_, "move");
    eventManager_.subscribe(&voiceAnnouncer_, "move");
    
    // רישום מאזינים לתפיסות 
    eventManager_.subscribe(&whiteScoreTracker_, "capture");
    eventManager_.subscribe(&blackScoreTracker_, "capture");
    eventManager_.subscribe(&voiceAnnouncer_, "capture");
    eventManager_.subscribe(&soundManager_, "capture");
    
    // רישום מאזינים למצב המשחק
    eventManager_.subscribe(&voiceAnnouncer_, "gamestate");
    eventManager_.subscribe(&soundManager_, "gamestate");
    
    std::cout << "Event listeners initialized successfully!" << std::endl;
}

std::string Game::cellToChessNotation(int x, int y) {
    char file = 'a' + y;  // y coordinate becomes file (a-h)
    int rank = x + 1;     // x coordinate becomes rank (1-8)
    return std::string(1, file) + std::to_string(rank);
}

void Game::drawGameInterface(ImgPtr background_img) {
    if (!background_img) {
        return;
    }
    
    // Get moves from trackers
    const auto& whiteMoves = whiteMovesTracker_.getAllMoves();
    const auto& blackMoves = blackMovesTracker_.getAllMoves();
    
    // Get scores
    int whiteScore = whiteScoreTracker_.getScore();
    int blackScore = blackScoreTracker_.getScore();
    
    // Background dimensions - עדכנתי לגודל הגדול יותר
    int bg_width = 1280;
    int bg_height = 960;
    
    // Board will be in center (640x640)
    int board_x = (bg_width - 640) / 2;  // Center horizontally
    int board_y = (bg_height - 640) / 2; // Center vertically
    
    // White moves table (right side)
    int white_table_x = board_x + 640 + 20;  // 20px margin from board
    int white_table_y = board_y;
    
    // Black moves table (left side)  
    int black_table_x = 20;  // 20px from left edge
    int black_table_y = board_y;
    
    // Draw tables
    drawMovesTable(background_img, white_table_x, white_table_y, whiteMoves, "White Moves", true);
    drawMovesTable(background_img, black_table_x, black_table_y, blackMoves, "Black Moves", false);
    
    // Draw scores above tables instead of below
    drawScore(background_img, white_table_x, white_table_y - 40, whiteScore, "White Score", true);
    drawScore(background_img, black_table_x, black_table_y - 40, blackScore, "Black Score", false);
    
    // NOTE: drawMessage is called AFTER everything else to ensure it appears on top
}

void Game::drawMovesTable(ImgPtr img, int x, int y, const std::vector<MoveEvent>& moves, const std::string& title, bool isWhite) {
    if (!img) return;
    
    // Table dimensions - 20 שורות קבועות
    int table_width = 170;
    int header_height = 30;
    int row_height = 22;
    int maxRows = 20;  // תמיד 20 שורות
    int table_height = header_height + (maxRows * row_height);
    
    // Column widths
    int time_col_width = 75;
    int move_col_width = 95;
    
    // Colors (RGB) - צבעים נכונים
    std::vector<uint8_t> white_bg = {255, 255, 255};      // White background
    std::vector<uint8_t> header_bg = {220, 220, 220};     // Light gray header
    std::vector<uint8_t> alt_row_bg = {245, 245, 245};    // Light gray for alternating rows
    std::vector<uint8_t> border_color = {0, 0, 0};        // Black borders
    std::vector<uint8_t> text_color = {0, 0, 0};          // Black text
    
    // Draw title above table
    img->put_text(title, x, y - 15, 0.7);
    
    // Draw main table background - רקע לבן אמיתי
    img->draw_rect(x, y, table_width, table_height, white_bg);
    
    // Draw header background
    img->draw_rect(x, y, table_width, header_height, header_bg);
    
    // Draw header text
    img->put_text("Time", x + 8, y + 20, 0.5);
    img->put_text("Move", x + time_col_width + 8, y + 20, 0.5);
    
    // Draw table borders - גבולות דקים ויפים
    img->draw_rect(x, y, table_width, 1, border_color);                    // Top border - דק
    img->draw_rect(x, y + header_height, table_width, 1, border_color);    // Header bottom - דק
    img->draw_rect(x, y, 1, table_height, border_color);                   // Left border - דק
    img->draw_rect(x + table_width - 1, y, 1, table_height, border_color); // Right border - דק
    img->draw_rect(x + time_col_width, y, 1, table_height, border_color);  // Column separator - דק
    
    // Draw moves - תמיד 20 שורות, רק עם התזוזות האחרונות
    int totalMoves = static_cast<int>(moves.size());
    int startIdx = (std::max)(0, totalMoves - maxRows);  // התחל מ-20 האחרונות
    
    for (int i = 0; i < maxRows; ++i) {
        int row_y = y + header_height + (i * row_height);
        
        // Draw alternating row background for all rows
        if (i % 2 == 1) {
            img->draw_rect(x + 1, row_y, table_width - 2, row_height, alt_row_bg);
        }
        // אחרת השורה תישאר לבנה (הרקע הלבן של הטבלה)
        
        // Draw horizontal row separator - דק
        if (i > 0) {
            img->draw_rect(x, row_y, table_width, 1, border_color);
        }
        
        // Draw content only if we have a move for this row
        if (startIdx + i < totalMoves) {
            const auto& move = moves[startIdx + i];
            
            // Format timestamp as MM:SS.mmm
            double timeSeconds = move.timestamp / 1000.0;
            int minutes = static_cast<int>(timeSeconds) / 60;
            int seconds = static_cast<int>(timeSeconds) % 60;
            int milliseconds = static_cast<int>((timeSeconds - static_cast<int>(timeSeconds)) * 1000);
            
            char timeStr[32];
            sprintf(timeStr, "%02d:%02d.%03d", minutes, seconds, milliseconds);
            
            // Extract piece notation in standard chess format
            std::string moveNotation;
            if (move.piece.length() >= 1) {
                char pieceType = move.piece[0];
                if (pieceType == 'P') {
                    // For pawns: just show destination (e.g., "e4")
                    moveNotation = move.to;
                } else {
                    // For other pieces: show piece type + destination (e.g., "Nf3", "Qd4")
                    moveNotation = std::string(1, pieceType) + move.to;
                }
            } else {
                // Fallback: show from->to format
                moveNotation = move.from + "->" + move.to;
            }
            
            // Draw text in cells
            img->put_text(timeStr, x + 6, row_y + 16, 0.4);
            img->put_text(moveNotation, x + time_col_width + 6, row_y + 16, 0.4);
        }
    }
    
    // Draw bottom border - דק ויפה
    img->draw_rect(x, y + table_height - 1, table_width, 1, border_color);
}

void Game::drawScore(ImgPtr img, int x, int y, int score, const std::string& player, bool isWhite) {
    if (!img) return;
    
    std::string scoreText = player + ": " + std::to_string(score) + " pts";
    img->put_text(scoreText, x, y, 0.8);
}

ImgPtr Game::loadBackgroundImage(const std::string& pieces_root, ImgFactoryPtr img_factory) {
    std::string bg_path = pieces_root + "background.jpg";
    return img_factory->load(bg_path, {1280, 960}); // Updated size for larger interface
}

Game create_game(const std::string& pieces_root, ImgFactoryPtr img_factory) {
    // Load board image
    std::string board_img_path = pieces_root + "board.png";
    auto board_img = img_factory->load(board_img_path, {640, 640});
    if (!board_img) {
        throw std::runtime_error("Failed to load board image: " + board_img_path);
    }
    
    // Load background image
    std::string bg_img_path = pieces_root + "background.jpg";
    auto background_img = img_factory->load(bg_img_path, {1280, 960});
    
    // Create board (8x8 chess board)
    Board board(80, 80, 8, 8, board_img);
    
    // Create graphics factory
    GraphicsFactory gfx_factory(img_factory);
    
    // Create piece factory
    PieceFactory piece_factory(board, pieces_root, gfx_factory);
    
    // Load pieces from board.csv
    std::string board_csv_path = pieces_root + "board.csv";
    auto pieces = piece_factory.create_pieces_from_board_csv(board_csv_path);
    
    // Create game with background if available
    if (background_img) {
        return Game(pieces, board, background_img);
    } else {
        std::cout << "Warning: Background image not found, using board-only display" << std::endl;
        return Game(pieces, board);
    }
}

void Game::showMessage(const std::string& message, int duration) {
    currentMessage_ = message;
    messageStartTime_ = game_time_ms();
    messageDuration_ = duration;
    showMessage_ = true;
    std::cout << "Game Message: " << message << std::endl;
}

void Game::drawMessage(ImgPtr img) {
    if (!img || !showMessage_) {
        return;
    }
    
    // Center of screen for message display
    int bg_width = 1280;
    int bg_height = 960;
    
    // Position message in CENTER of screen (over the board)
    int text_x = bg_width / 2 - (currentMessage_.length() * 8); // Smaller font adjustment
    int text_y = bg_height / 2; // Center vertically
    
    // Draw text multiple times with slight offsets to create bold/thick effect
    // Using smaller offsets and smaller font for better visibility
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            img->put_text(currentMessage_, text_x + dx, text_y + dy, 1.5);  // גופן קטן יותר שיכנס במסך
        }
    }
}

void Game::updateMessageDisplay() {
    if (showMessage_ && messageStartTime_ != -1) {
        int currentTime = game_time_ms();
        if (currentTime - messageStartTime_ >= messageDuration_) {
            showMessage_ = false;
            messageStartTime_ = -1;
            currentMessage_.clear();
        }
    }
}

void Game::onGameStateChanged(const GameStateEvent& event) {
    switch (event.newState) {
        case GameState::Playing:
            if (event.previousState == GameState::Paused) {
                showMessage("Game Starting! Good luck!", 5000);  // הגדלתי ל-5 שניות
            }
            break;
        case GameState::WhiteWin:
            showMessage("White Team Wins! Congratulations!", 2000);  // קיצרתי ל-2 שניות
            break;
        case GameState::BlackWin:
            showMessage("Black Team Wins! Congratulations!", 2000);  // קיצרתי ל-2 שניות
            break;
        case GameState::Draw:
            showMessage("It's a Draw! Great game!", 2000);  // קיצרתי ל-2 שניות
            break;
        case GameState::Paused:
            showMessage("Game Paused", 3000);  // הגדלתי ל-3 שניות
            break;
    }
    
    if (!event.reason.empty()) {
        // Add reason to current message if exists
        std::string reasonMessage = currentMessage_ + " - " + event.reason;
        showMessage(reasonMessage, messageDuration_);
    }
}

// Network move handling functions
void Game::broadcastMove(const std::string& move) {
    if (network_interface_) {
        std::cout << "🌐 Broadcasting move: " << move << std::endl;
        network_interface_->sendMove(move);
    }
}

void Game::applyNetworkMove(const std::string& move) {
    std::cout << "🌐 Applying network move: " << move << std::endl;
    
    // Parse move format: "piece_id:from_x,from_y:to_x,to_y"
    size_t first_colon = move.find(':');
    size_t second_colon = move.find(':', first_colon + 1);
    
    if (first_colon == std::string::npos || second_colon == std::string::npos) {
        std::cerr << "❌ Invalid move format: " << move << std::endl;
        return;
    }
    
    std::string piece_id = move.substr(0, first_colon);
    std::string from_pos = move.substr(first_colon + 1, second_colon - first_colon - 1);
    std::string to_pos = move.substr(second_colon + 1);
    
    // Parse from position
    size_t comma = from_pos.find(',');
    if (comma == std::string::npos) {
        std::cerr << "❌ Invalid from position: " << from_pos << std::endl;
        return;
    }
    int from_x = std::stoi(from_pos.substr(0, comma));
    int from_y = std::stoi(from_pos.substr(comma + 1));
    
    // Parse to position
    comma = to_pos.find(',');
    if (comma == std::string::npos) {
        std::cerr << "❌ Invalid to position: " << to_pos << std::endl;
        return;
    }
    int to_x = std::stoi(to_pos.substr(0, comma));
    int to_y = std::stoi(to_pos.substr(comma + 1));
    
    // Find the piece and apply the move
    auto piece = find_piece_by_id(piece_id);
    if (piece) {
        std::vector<std::pair<int,int>> move_params = {{from_x, from_y}, {to_x, to_y}};
        Command move_cmd(game_time_ms(), piece_id, "move", move_params, 1);
        enqueue_command(move_cmd);
        std::cout << "✅ Applied move for piece " << piece_id << " from (" << from_x << "," << from_y << ") to (" << to_x << "," << to_y << ")" << std::endl;
    } else {
        std::cerr << "❌ Piece not found: " << piece_id << std::endl;
    }
}

// Server-side input processing (calls local processing without network)
void Game::processServerInput(int player_id, const std::string& cmd_type) {
    process_input_local(player_id, cmd_type);
}
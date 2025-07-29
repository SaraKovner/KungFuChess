#include "Game.hpp"
#include <opencv2/opencv.hpp>
#include <set>
#include "Physics.hpp"

// ---------------- Implementation --------------------
Game::Game(std::vector<PiecePtr> pcs, Board board)
    : pieces(pcs), board(board) {
    validate();
    for(const auto & p : pieces) {
        if (p) {
            piece_by_id[p->id] = p;
        }
    }
    start_tp = std::chrono::steady_clock::now();
    // Initialize position map
    update_cell2piece_map();
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
    // Don't call reset - it breaks piece positions
    // for(auto & p : pieces) p->reset(start_ms);

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
        
        // Update all pieces
        for(auto & p : pieces) {
            p->update(now);
        }

        update_cell2piece_map();

        // Input processing moved to graphics section where OpenCV handles keys

        if(is_with_graphics) {
            // Create a copy of the board to draw pieces on
            auto display_board = board.clone();
            
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
                    if (piece->state->name == "move") {
                        pos_pix = piece->state->physics->get_pos_pix();
                        // Debug: print position during movement
                        auto pos_m = piece->state->physics->get_pos_m();
                        std::cout << "Moving piece at: (" << pos_m.first << ", " << pos_m.second << ") -> pix: (" << pos_pix.first << ", " << pos_pix.second << ")" << std::endl;
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
                // Draw green border around current cursor position
                auto cursor_pos_m = display_board.cell_to_m(cursor_pos_);
                auto cursor_pos_pix = display_board.m_to_pix(cursor_pos_m);
                int cell_size = 80;
                display_board.img->draw_rect(cursor_pos_pix.first, cursor_pos_pix.second, 
                                           cell_size, cell_size, {0, 255, 0}); // Green border
                
                // Draw blue border around selected piece
                if (selected_piece_) {
                    auto selected_pos_m = display_board.cell_to_m(selected_piece_pos_);
                    auto selected_pos_pix = display_board.m_to_pix(selected_pos_m);
                    display_board.img->draw_rect(selected_pos_pix.first, selected_pos_pix.second, 
                                               cell_size, cell_size, {255, 0, 0}); // Blue border
                }
                
                display_board.show();
                
                // Handle input in main loop where window exists
                int key = cv::waitKeyEx(30);
                if (key != -1) {
                    Command cmd(game_time_ms(), "", "", {});
                    
                    // Arrow keys controls (final: swap 8 and 2)
                    if (key == 2424832) cmd = Command(game_time_ms(), "", "up", {});    // 4 -> Up
                    else if (key == 2555904) cmd = Command(game_time_ms(), "", "down", {});  // 6 -> Down  
                    else if (key == 2490368) cmd = Command(game_time_ms(), "", "left", {});  // 8 -> Left
                    else if (key == 2621440) cmd = Command(game_time_ms(), "", "right", {}); // 2 -> Right
                    else if (key == 13) cmd = Command(game_time_ms(), "", "select", {});  // Enter
                    else if (key == 27) { // ESC
                        return;
                    }
                    
                    if (!cmd.type.empty()) {
                        process_input(cmd);
                    }
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

void Game::process_input(const Command& cmd) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    
    if (cmd.type == "up") move_cursor(0, -1);
    else if (cmd.type == "down") move_cursor(0, 1);
    else if (cmd.type == "left") move_cursor(-1, 0);
    else if (cmd.type == "right") move_cursor(1, 0);
    else if (cmd.type == "select") {
        // Update position map before accessing it
        update_cell2piece_map();
        
        // Enhanced selection logic from movement-logic branch
        if (selected_piece_ == nullptr) {
            // First press - pick up piece under cursor
            auto cell_pieces_it = pos.find(cursor_pos_);
            if (cell_pieces_it != pos.end() && !cell_pieces_it->second.empty()) {
                selected_piece_ = cell_pieces_it->second[0];
                selected_piece_pos_ = cursor_pos_;
            }
        } else if (cursor_pos_ == selected_piece_pos_) {
            // Same position - deselect
            selected_piece_ = nullptr;
            selected_piece_pos_ = {-1, -1};
        } else {
            // Different position - validate and create move command
            if (is_move_valid(selected_piece_, selected_piece_pos_, cursor_pos_)) {
                try {
                    Command move_cmd(cmd.timestamp, selected_piece_->id, "move", {selected_piece_pos_, cursor_pos_});
                    
                    // Process the move command through state machine
                    auto piece_it = piece_by_id.find(move_cmd.piece_id);
                    if (piece_it != piece_by_id.end()) {
                        auto piece = piece_it->second;
                        if (piece && piece->state) {
                            // Update position map again before passing to piece
                            update_cell2piece_map();
                            piece->on_command(move_cmd, pos);
                        }
                    }
                } catch (const std::exception& e) {
                    // Handle move command errors silently
                }
            }
            // If move is invalid, silently ignore (piece stays selected)
            
            // Reset selection
            selected_piece_ = nullptr;
            selected_piece_pos_ = {-1, -1};
        }
    }
}

void Game::move_cursor(int dx, int dy) {
    cursor_pos_.first = std::max(0, std::min(board.W_cells - 1, cursor_pos_.first + dx));
    cursor_pos_.second = std::max(0, std::min(board.H_cells - 1, cursor_pos_.second + dy));
}

void Game::resolve_collisions() {
    check_captures();
}

void Game::announce_win() const {
    // Game end
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
    if (!is_selecting_target_) {
        select_piece_at(x, y);
    } else {
        if (selected_piece_) {
            auto start_cell = selected_piece_->current_cell();
            if (is_move_valid(selected_piece_, start_cell, {x, y})) {
                Command move_cmd(game_time_ms(), selected_piece_->id, "move", {start_cell, {x, y}}, 1);
                enqueue_command(move_cmd);
            }
        }
        cancel_selection();
    }
}

void Game::handle_key_press(int key) {
    std::lock_guard<std::mutex> lock(input_mutex_);
    switch(key) {
        case 27: cancel_selection(); break;
        case 13: confirm_move(); break;
        case 2424832: move_cursor(0, -1); break;  // 4 -> Up
        case 2555904: move_cursor(0, 1); break;   // 6 -> Down
        case 2490368: move_cursor(-1, 0); break;  // 8 -> Left
        case 2621440: move_cursor(1, 0); break;   // 2 -> Right
    }
}

void Game::select_piece_at(int x, int y) {
    auto cell_pieces_it = pos.find({x, y});
    if (cell_pieces_it != pos.end() && !cell_pieces_it->second.empty()) {
        selected_piece_ = cell_pieces_it->second[0];
        cursor_pos_ = {x, y};
        is_selecting_target_ = true;
    }
}

void Game::confirm_move() {
    if (selected_piece_ && is_selecting_target_) {
        auto start_cell = selected_piece_->current_cell();
        if (is_move_valid(selected_piece_, start_cell, cursor_pos_)) {
            Command move_cmd(game_time_ms(), selected_piece_->id, "move", {start_cell, cursor_pos_}, 1);
            enqueue_command(move_cmd);
        }
    }
    cancel_selection();
}

void Game::cancel_selection() {
    selected_piece_ = nullptr;
    is_selecting_target_ = false;
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
            std::cout << "=== COLLISION DETECTED AT CELL (" << cell.first << "," << cell.second << ") ===" << std::endl;
            std::cout << "PIECES IN COLLISION:" << std::endl;
            for (size_t k = 0; k < pieces_at_cell.size(); ++k) {
                auto p = pieces_at_cell[k];
                std::cout << "  [" << k << "] " << p->id << " - state: " << p->state->name 
                          << ", start_ms: " << p->state->physics->start_ms << std::endl;
            }
            
            for (size_t i = 0; i < pieces_at_cell.size(); ++i) {
                for (size_t j = i + 1; j < pieces_at_cell.size(); ++j) {
                    auto piece1 = pieces_at_cell[i];
                    auto piece2 = pieces_at_cell[j];
                    
                    // Skip if either piece is already captured
                    if (already_captured.count(piece1->id) || already_captured.count(piece2->id)) {
                        continue;
                    }
                    
                    // Create collision key to avoid duplicate reports
                    std::string key1 = piece1->id + "+" + piece2->id;
                    std::string key2 = piece2->id + "+" + piece1->id;
                    
                    // Validate pieces before accessing their states
                    if (piece1 && piece2 && piece1->state && piece2->state && 
                        piece1->id.length() >= 2 && piece2->id.length() >= 2) {
                        
                        // בדיקת צוות - האות השנייה בID (W או B)
                        char piece1_team = piece1->id[1];
                        char piece2_team = piece2->id[1];
                        
                        // Only print if this collision hasn't been reported yet
                        if (reported_collisions.find({key1, key2}) == reported_collisions.end() && 
                            reported_collisions.find({key2, key1}) == reported_collisions.end()) {
                            std::cout << "COLLISION: " << piece1->id << " (team " << piece1_team 
                                      << ") vs " << piece2->id << " (team " << piece2_team << ")" << std::endl;
                            reported_collisions.insert({key1, key2});
                        }
                        
                        if (piece1_team == piece2_team) {
                            // שני כלים מאותו צוות - לא לעשות כלום!
                            std::cout << "SAME TEAM BLOCKED: " << piece1->id << " and " << piece2->id << " - NO ACTION" << std::endl;
                            continue;
                        }
                        
                        // רק אם הם מצוותים שונים - בדוק מי התוקף ומי הקורבן
                        // DEBUG: מצבי החתיכות ופיזיקה מפורטת
                        
                        std::cout << "\n--- DETAILED PIECE ANALYSIS ---" << std::endl;
                        
                        // פרטי piece1
                        std::cout << piece1->id << ":" << std::endl;
                        std::cout << "  Current state: " << piece1->state->name << std::endl;
                        std::cout << "  Physics start_ms: " << piece1->state->physics->start_ms << std::endl;
                        std::cout << "  Physics type: " << typeid(*(piece1->state->physics)).name() << std::endl;
                        if (piece1->state->name == "move") {
                            auto move_phys1 = std::dynamic_pointer_cast<MovePhysics>(piece1->state->physics);
                            if (move_phys1) {
                                std::cout << "  Move duration: " << move_phys1->get_duration_s() << "s" << std::endl;
                                std::cout << "  Estimated arrival: " << (piece1->state->physics->start_ms + (int)(move_phys1->get_duration_s() * 1000)) << "ms" << std::endl;
                            }
                        }
                        
                        // פרטי piece2  
                        std::cout << piece2->id << ":" << std::endl;
                        std::cout << "  Current state: " << piece2->state->name << std::endl;
                        std::cout << "  Physics start_ms: " << piece2->state->physics->start_ms << std::endl;
                        std::cout << "  Physics type: " << typeid(*(piece2->state->physics)).name() << std::endl;
                        if (piece2->state->name == "move") {
                            auto move_phys2 = std::dynamic_pointer_cast<MovePhysics>(piece2->state->physics);
                            if (move_phys2) {
                                std::cout << "  Move duration: " << move_phys2->get_duration_s() << "s" << std::endl;
                                std::cout << "  Estimated arrival: " << (piece2->state->physics->start_ms + (int)(move_phys2->get_duration_s() * 1000)) << "ms" << std::endl;
                            }
                        }
                        
                        std::cout << "Current game time: " << game_time_ms() << "ms" << std::endl;
                        std::cout << "--- END ANALYSIS ---\n" << std::endl;
                        
                        bool piece1_is_moving = (piece1->state->name == "move");
                        bool piece2_is_moving = (piece2->state->name == "move");
                        
                        // זמן הגעה אמיתי לתא - start_ms + duration עבור חתיכות שזזו
                        int piece1_arrival_time = piece1->state->physics->start_ms;
                        int piece2_arrival_time = piece2->state->physics->start_ms;
                        
                        // אם החתיכה בmove, נחשב מתי היא תגיע/הגיעה
                        if (piece1_is_moving) {
                            auto move_physics1 = std::dynamic_pointer_cast<MovePhysics>(piece1->state->physics);
                            if (move_physics1) {
                                piece1_arrival_time += (int)(move_physics1->get_duration_s() * 1000);
                            }
                        }
                        
                        if (piece2_is_moving) {
                            auto move_physics2 = std::dynamic_pointer_cast<MovePhysics>(piece2->state->physics);
                            if (move_physics2) {
                                piece2_arrival_time += (int)(move_physics2->get_duration_s() * 1000);
                            }
                        }
                        
                        std::cout << "ANALYSIS: " << piece1->id << " (state=" << piece1->state->name 
                                  << ", arrival_time=" << piece1_arrival_time << "ms) vs " 
                                  << piece2->id << " (state=" << piece2->state->name 
                                  << ", arrival_time=" << piece2_arrival_time << "ms)" << std::endl;
                        
                        PiecePtr attacker = nullptr;
                        PiecePtr victim = nullptr;
                        
                        // בשחמט: מי שמגיע למקום (זמן הגעה מאוחר יותר) אוכל את מי שכבר נמצא במקום (זמן הגעה מוקדם יותר)
                        if (piece1_arrival_time > piece2_arrival_time) {
                            attacker = piece1;  // הגיע אחרון = מגיע עכשיו = תוקף
                            victim = piece2;    // הגיע קודם = היה במקום = קורבן
                            std::cout << "ATTACKER: " << piece1->id << " (הגיע " << piece1_arrival_time 
                                      << "ms) לוכד VICTIM: " << piece2->id << " (היה במקום מ-" << piece2_arrival_time << "ms)" << std::endl;
                            
                        } else if (piece2_arrival_time > piece1_arrival_time) {
                            attacker = piece2;  // הגיע אחרון = מגיע עכשיו = תוקף
                            victim = piece1;    // הגיע קודם = היה במקום = קורבן
                            std::cout << "ATTACKER: " << piece2->id << " (הגיע " << piece2_arrival_time 
                                      << "ms) לוכד VICTIM: " << piece1->id << " (היה במקום מ-" << piece1_arrival_time << "ms)" << std::endl;
                            
                        } else {
                            // הגיעו באותו זמן - בדוק מי יצא קודם (זמן התחלת המסע)
                            std::cout << "SIMULTANEOUS ARRIVAL: " << piece1->id << " and " << piece2->id 
                                      << " both arrived at " << piece1_arrival_time << "ms" << std::endl;
                            std::cout << "CHECKING START TIMES: " << piece1->id << " started at " 
                                      << piece1->state->physics->start_ms << "ms vs " << piece2->id 
                                      << " started at " << piece2->state->physics->start_ms << "ms" << std::endl;
                            
                            if (piece1->state->physics->start_ms < piece2->state->physics->start_ms) {
                                attacker = piece1;  // יצא קודם = עדיפות
                                victim = piece2;
                                std::cout << "ATTACKER: " << piece1->id << " (יצא קודם " << piece1->state->physics->start_ms 
                                          << "ms) לוכד VICTIM: " << piece2->id << " (יצא אחר כך " << piece2->state->physics->start_ms << "ms)" << std::endl;
                            } else if (piece2->state->physics->start_ms < piece1->state->physics->start_ms) {
                                attacker = piece2;  // יצא קודם = עדיפות
                                victim = piece1;
                                std::cout << "ATTACKER: " << piece2->id << " (יצא קודם " << piece2->state->physics->start_ms 
                                          << "ms) לוכד VICTIM: " << piece1->id << " (יצא אחר כך " << piece1->state->physics->start_ms << "ms)" << std::endl;
                            } else {
                                // גם יצאו באותו זמן - אין לכידה!
                                std::cout << "PERFECT TIE: Both pieces started and arrived at exactly the same time - NO CAPTURE" << std::endl;
                                continue;
                            }
                        }
                        
                        // בדוק אם התוקף יכול לאכול את הקורבן
                        // בקונג-פו שח: מי שמגיע למקום עם מישהו אחר תמיד נלחם!
                        if (attacker && victim) {
                            std::cout << "COMBAT: " << attacker->id << " (earlier arrival) fights " << victim->id << std::endl;
                            std::cout << "COMBAT RESULT: " << attacker->id << " wins and captures " << victim->id << std::endl;
                            already_captured.insert(victim->id);
                            capture_piece(victim, attacker);
                            return; // Exit completely after capture
                        } else {
                            std::cout << "ERROR: Invalid attacker/victim pointers" << std::endl;
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
    
    // Update position map to ensure accuracy
    update_cell2piece_map();
    
    // Create set of occupied cells for path checking
    std::unordered_set<std::pair<int,int>, PairHash> occupied_cells;
    for (const auto& [cell, pieces_at_cell] : pos) {
        if (!pieces_at_cell.empty()) {
            occupied_cells.insert(cell);
            std::cout << "OCCUPIED CELL: (" << cell.first << "," << cell.second << ") בה נמצא " << pieces_at_cell[0]->id << std::endl;
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

Game create_game(const std::string& pieces_root, ImgFactoryPtr img_factory) {
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
    
    return Game(pieces, board);
}
#pragma once

#include "Board.hpp"
#include "PieceFactory.hpp"
#include "Command.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <unordered_set>
#include "img/Img.hpp"
#include "img/ImgFactory.hpp"
#include <fstream>
#include <sstream>
#include "GraphicsFactory.hpp"
#include "Common.hpp"
#include "img/OpenCvImg.hpp"
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <opencv2/opencv.hpp>
// Threading support from CTD25_1
#include <mutex>
#include <condition_variable>
#include <atomic>

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#error "Filesystem support not found"
#endif

class InvalidBoard : public std::runtime_error {
public:
    explicit InvalidBoard(const std::string& msg) : std::runtime_error(msg) {}
};

class Game {
public:
    Game(std::vector<PiecePtr> pcs, Board board);

    // --- main public API ---
    int game_time_ms() const;
    Board clone_board() const;

    // Mirror Python run() behaviour with enhanced threading support
    void run(int num_iterations = -1, bool is_with_graphics = true);

    std::vector<PiecePtr> pieces;
    Board board;
    
    // helper for tests to inject commands
    void enqueue_command(const Command& cmd);

private:
    // --- helpers mirroring Python implementation ---
    void start_user_input_thread();
    void run_game_loop(int num_iterations, bool is_with_graphics);
    void update_cell2piece_map();
    void process_input(const Command& cmd);
    void resolve_collisions();
    void announce_win() const;

    void validate();
    bool is_win() const;

    std::unordered_map<std::string, PiecePtr> piece_by_id;
    // Map from board cell to list of occupying pieces
    std::unordered_map<std::pair<int,int>, std::vector<PiecePtr>, PairHash> pos;
    
    // Enhanced threading support from CTD25_1
    std::queue<Command> user_input_queue;
    std::queue<std::string> string_input_queue;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::mutex positions_mutex_;
    std::mutex input_mutex_;
    
    // Selected piece for user interaction
    PiecePtr selected_piece_ = nullptr;
    std::pair<int, int> cursor_pos_ = {0, 0};
    std::pair<int, int> selected_piece_pos_ = {-1, -1};
    bool is_selecting_target_ = false;
    int current_player_ = 1;  // Current active player

    std::chrono::steady_clock::time_point start_tp;
    
    // Helper functions for user interaction
    void handle_mouse_click(int x, int y);
    void handle_key_press(int key);
    void select_piece_at(int x, int y);
    void move_cursor(int dx, int dy);
    void confirm_move();
    void cancel_selection();
    std::string cell_to_chess_notation(int x, int y);
    PiecePtr find_piece_by_id(const std::string& id);
    void check_captures();
    void capture_piece(PiecePtr captured, PiecePtr captor);
    std::string get_position_key(int x, int y);
    bool is_move_valid(PiecePtr piece, const std::pair<int,int>& from, const std::pair<int,int>& to);
    char get_piece_color(PiecePtr piece);
    bool are_same_color(PiecePtr piece1, PiecePtr piece2);
};

// Factory function to create game from pieces directory
Game create_game(const std::string& pieces_root, ImgFactoryPtr img_factory);
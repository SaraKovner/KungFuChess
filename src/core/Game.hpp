#pragma once
#include "../graphics/img/ImgFactory.hpp"
#include "Common.hpp"
#include "../graphics/img/OpenCvImg.hpp"
#include "../network/NetworkInterface.hpp"
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Board.hpp"
#include "../game_logic/PieceFactory.hpp"
#include "../ui/Command.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include "../graphics/img/Img.hpp"
#include "../graphics/img/ImgFactory.hpp"
#include <fstream>
#include <sstream>
#include "../graphics/GraphicsFactory.hpp"
// Threading support from CTD25_1
#include <mutex>
#include <condition_variable>
#include <atomic>

// Observer pattern for game events - Updated architecture
#include "../observer/headers/GameEventManager.hpp"
#include "../observer/headers/GameStateEvent.hpp"
#include "../observer/headers/WhiteMovesTable.hpp"
#include "../observer/headers/BlackMovesTable.hpp"
#include "../observer/headers/WhiteScoreTracker.hpp"
#include "../observer/headers/BlackScoreTracker.hpp"
#include "../observer/headers/VoiceAnnouncer.hpp"
#include "../observer/headers/SoundManager.hpp"

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
    Game(std::vector<PiecePtr> pcs, Board board, ImgPtr background_img);

    // --- main public API ---
    int game_time_ms() const;
    Board clone_board() const;

    // Mirror Python run() behaviour with enhanced threading support
    void run(int num_iterations = -1, bool is_with_graphics = true);
    
    // Network interface for multiplayer
    void setNetworkInterface(NetworkInterface* network);
    void setMyPlayerId(int player_id); // Set which player this client represents
    
    // Network move handling
    void applyNetworkMove(const std::string& move);
    void broadcastMove(const std::string& move);
    
    // Server-side input processing (public for GameServer)
    void processServerInput(int player_id, const std::string& cmd_type);

    std::vector<PiecePtr> pieces;
    Board board;
    
    // Background image for game interface
    ImgPtr background_img_;
    
    // helper for tests to inject commands
    void enqueue_command(const Command& cmd);

private:
    // --- helpers mirroring Python implementation ---
    void start_user_input_thread();
    void run_game_loop(int num_iterations, bool is_with_graphics);
    void update_cell2piece_map();
    void process_input(int player_id, const std::string& cmd_type);
    void process_input_local(int player_id, const std::string& cmd_type);  // מעבד קלט מקומית
    void resolve_collisions();
    void announce_win();

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
    
    // Player 1 state (White - Arrow keys + Enter)
    PiecePtr selected_piece_player1_ = nullptr;
    std::pair<int, int> cursor_pos_player1_ = {0, 0};
    std::pair<int, int> selected_piece_pos_player1_ = {-1, -1};
    bool is_selecting_target_player1_ = false;
    
    // Player 2 state (Black - WASD + Space)
    PiecePtr selected_piece_player2_ = nullptr;
    std::pair<int, int> cursor_pos_player2_ = {7, 7};
    std::pair<int, int> selected_piece_pos_player2_ = {-1, -1};
    bool is_selecting_target_player2_ = false;
    
    // Key mappings for both players
    std::unordered_map<int, std::string> keymap_player1_;
    std::unordered_map<int, std::string> keymap_player2_;

    std::chrono::steady_clock::time_point start_tp;
    
    // Game state tracking
    GameState currentGameState_ = GameState::Paused;
    
    // Message display system
    std::string currentMessage_;
    int messageStartTime_ = -1;
    int messageDuration_ = 3000; // 3 seconds in milliseconds
    bool showMessage_ = false;
    
    // Event management for observer pattern
    GameEventManager eventManager_;
    
    // Event listeners
    WhiteMovesTable whiteMovesTracker_;
    BlackMovesTable blackMovesTracker_;
    WhiteScoreTracker whiteScoreTracker_;
    BlackScoreTracker blackScoreTracker_;
    VoiceAnnouncer voiceAnnouncer_;
    SoundManager soundManager_;
    
    // Network interface for multiplayer
    NetworkInterface* network_interface_ = nullptr;
    int my_player_id_ = -1; // -1 = both players (local game), 1 = white, 2 = black
    
    // Helper functions for user interaction
    void setupEventListeners();
    std::string cellToChessNotation(int x, int y);
    void handle_mouse_click(int x, int y);
    void handle_key_press(int key);
    void select_piece_at(int x, int y);
    void move_cursor(int player_id, int dx, int dy);
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
    
    // Visual display functions for move tables and scores
    void drawGameInterface(ImgPtr background_img);
    void drawMovesTable(ImgPtr img, int x, int y, const std::vector<MoveEvent>& moves, const std::string& title, bool isWhite);
    void drawScore(ImgPtr img, int x, int y, int score, const std::string& player, bool isWhite);
    
    // Message display functions
    void showMessage(const std::string& message, int duration = 3000);
    void drawMessage(ImgPtr img);
    void updateMessageDisplay();
    
    // Game state event handler
    void onGameStateChanged(const GameStateEvent& event);
    
    // Static functions
    static ImgPtr loadBackgroundImage(const std::string& pieces_root, ImgFactoryPtr img_factory);
};

// Factory function to create game from pieces directory
Game create_game(const std::string& pieces_root, ImgFactoryPtr img_factory);
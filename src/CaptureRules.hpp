#pragma once

#include "Piece.hpp"
#include "Physics.hpp"
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <functional>

/**
 * Helper functions for capture rules and combat resolution in KungFu Chess.
 * These functions handle the complex logic of determining attacker/victim relationships
 * based on arrival times and team validation.
 */
class CaptureRules {
public:
    // Print collision summary when multiple pieces occupy the same cell
    static void print_collision_summary(const std::pair<int,int>& cell, const std::vector<PiecePtr>& pieces);
    
    // Check if two pieces belong to the same team (W or B)
    static bool are_same_team(PiecePtr piece1, PiecePtr piece2);
    
    // Print detailed analysis of a piece's state and physics
    static void print_piece_analysis(PiecePtr piece, int current_game_time);
    
    // Calculate when a piece arrived/will arrive at its current cell
    static int calculate_arrival_time(PiecePtr piece);
    
    // Determine which piece is attacker and which is victim based on arrival times
    static std::pair<PiecePtr, PiecePtr> determine_attacker_and_victim(PiecePtr piece1, PiecePtr piece2);
    
    // Resolve conflicts when two pieces arrive at exactly the same time
    static std::pair<PiecePtr, PiecePtr> resolve_simultaneous_arrival(PiecePtr piece1, PiecePtr piece2, int arrival_time);
    
    // Process a collision between two pieces, returns true if capture occurred
    static bool process_collision_pair(PiecePtr piece1, PiecePtr piece2, 
                                      std::set<std::string>& already_captured,
                                      std::set<std::pair<std::string, std::string>>& reported_collisions,
                                      int current_game_time,
                                      std::function<void(PiecePtr, PiecePtr)> capture_callback);
};

#pragma once

#include "Board.hpp"
#include "Command.hpp"
#include "Common.hpp"
#include <cmath>
#include <memory>
#include <iostream>

class BasePhysics {
public:
    explicit BasePhysics(const Board& board, double param = 1.0)
        : board(board), param(param) {}

    virtual ~BasePhysics() = default;

    virtual void reset(const Command& cmd) = 0;
    // Update physics state. Return a Command if one is produced, otherwise nullptr
    virtual std::shared_ptr<Command> update(int now_ms) = 0;

    std::pair<double,double> get_pos_m() const { return curr_pos_m; }
    std::pair<int,int> get_pos_pix() const { 
        double x_m = curr_pos_m.first;
        double y_m = curr_pos_m.second;
        
        // Use safe pixel values if board is corrupted
        int safe_cell_W_pix = (board.cell_W_pix <= 0) ? 80 : board.cell_W_pix;
        int safe_cell_H_pix = (board.cell_H_pix <= 0) ? 80 : board.cell_H_pix;
        
        int x_px = static_cast<int>(std::round(x_m * safe_cell_W_pix));
        int y_px = static_cast<int>(std::round(y_m * safe_cell_H_pix));
        
        return {x_px, y_px};
    }
    std::pair<int,int> get_curr_cell() const { return board.m_to_cell(curr_pos_m); }

    int get_start_ms() const { return start_ms; }

    virtual bool can_be_captured() const { return true; }
    virtual bool can_capture() const { return true; }
    virtual bool is_movement_blocker() const { return false; }

public:
    const Board& board;
    double param{1.0};

    std::pair<int,int> start_cell{0,0};
    std::pair<int,int> end_cell{0,0};
    std::pair<double,double> curr_pos_m{0.0,0.0};
    int start_ms{0};
};

// ---------------------------------------------------------------------------
class IdlePhysics : public BasePhysics {
public:
    using BasePhysics::BasePhysics;
    void reset(const Command& cmd) override {
        if(!cmd.params.empty()) {
            start_cell = end_cell = cmd.params[0];
            curr_pos_m = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        } else {
            // Fallback to origin if no parameters
            start_cell = end_cell = {0, 0};
            curr_pos_m = {0.0, 0.0};
        }
        start_ms = cmd.timestamp;
    }
    std::shared_ptr<Command> update(int) override { return nullptr; }

    bool can_capture() const override { return false; }
    bool is_movement_blocker() const override { return true; }
};

// ---------------------------------------------------------------------------
class MovePhysics : public BasePhysics {
public:
    explicit MovePhysics(const Board& board, double speed_cells_per_s)
        : BasePhysics(board, speed_cells_per_s) {}

    void reset(const Command& cmd) override {
        if (cmd.params.size() < 2) {
            // Invalid command, stay at current position
            std::cout << "MOVE: Invalid command - not enough parameters" << std::endl;
            return;
        }
        
        start_cell = cmd.params[0];
        end_cell   = cmd.params[1];
        curr_pos_m = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        start_ms   = cmd.timestamp;

        std::pair<double,double> start_pos = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        std::pair<double,double> end_pos = {static_cast<double>(end_cell.second), static_cast<double>(end_cell.first)};
        movement_vec = { end_pos.first - start_pos.first, end_pos.second - start_pos.second };
        movement_len = std::hypot(movement_vec.first, movement_vec.second);
        
        // Ensure we have a valid speed
        double speed_m_s = (param > 0.0) ? param : 0.5; // Default to 0.5 if invalid
        
        // Avoid division by zero
        if (movement_len > 0.0 && speed_m_s > 0.0) {
            duration_s = movement_len / speed_m_s;
        } else {
            duration_s = 0.1; // Minimum duration
        }
        
        std::cout << "MOVE: (" << start_cell.first << "," << start_cell.second 
                  << ") -> (" << end_cell.first << "," << end_cell.second << ") duration: " << duration_s << "s" << std::endl;
    }

    std::shared_ptr<Command> update(int now_ms) override {
        // Use safe board values if needed
        double safe_cell_H_m = (board.cell_H_m < 0.1) ? 1.0 : board.cell_H_m;
        double safe_cell_W_m = (board.cell_W_m < 0.1) ? 1.0 : board.cell_W_m;
        
        double seconds = (now_ms - start_ms) / 1000.0;
        if(seconds >= duration_s) {
            curr_pos_m = {static_cast<double>(end_cell.second), static_cast<double>(end_cell.first)};
            std::cout << "MOVE completed at: (" << curr_pos_m.first << "," << curr_pos_m.second << ")" << std::endl;
            return std::make_shared<Command>(Command{now_ms, "", "done", {end_cell}});
        }
        double ratio = seconds / duration_s;
        
        // Safe calculation with bounds checking
        if (ratio >= 0.0 && ratio <= 1.0 && duration_s > 0.0) {
            auto start_pos = std::make_pair(static_cast<double>(start_cell.second), static_cast<double>(start_cell.first));
            curr_pos_m = { start_pos.first + movement_vec.first * ratio,
                           start_pos.second + movement_vec.second * ratio };
        } else {
            // Fallback to start position if calculation is invalid
            curr_pos_m = std::make_pair(static_cast<double>(start_cell.second), static_cast<double>(start_cell.first));
        }
        return nullptr;
    }

private:
    std::pair<double,double> movement_vec{0.f,0.f};
    double movement_len{0};
    double duration_s{1.0};
public:
    double get_speed_m_s() const { return param; }
    double get_duration_s() const { return duration_s; }
}; // end MovePhysics

// ---------------------------------------------------------------------------
class StaticTemporaryPhysics : public BasePhysics {
public:
    using BasePhysics::BasePhysics;
    double get_duration_s() const { return param; }

    void reset(const Command& cmd) override {
        if(!cmd.params.empty()) {
            start_cell = end_cell = cmd.params[0];
            curr_pos_m = {static_cast<double>(start_cell.second), static_cast<double>(start_cell.first)};
        }
        start_ms = cmd.timestamp;
    }

    std::shared_ptr<Command> update(int now_ms) override {
        double seconds = (now_ms - start_ms) / 1000.0;
        if(seconds >= param) {
            return std::make_shared<Command>(Command{now_ms, "", "done", {end_cell}});
        }
        return nullptr;
    }

    bool is_movement_blocker() const override { return true; }
};

class JumpPhysics : public StaticTemporaryPhysics {
public:
    using StaticTemporaryPhysics::StaticTemporaryPhysics;
    bool can_be_captured() const override { return false; }
};

class RestPhysics : public StaticTemporaryPhysics {
public:
    using StaticTemporaryPhysics::StaticTemporaryPhysics;
    bool can_capture() const override { return false; }
};

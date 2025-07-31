#pragma once

#include <string>
#include <vector>
#include <ostream>

struct Command {
    int timestamp;                 // ms since game start
    std::string piece_id;          // identifier of the piece (may be empty)
    std::string type;              // e.g., "move", "jump", "done"...
    std::vector<std::pair<int,int>> params;  // payload – board cells etc.
    int player_id = 1;             // player identifier (1 or 2)

    Command(int ts, std::string pid, std::string t, std::vector<std::pair<int,int>> p, int player = 1)
        : timestamp(ts), piece_id(pid), type(t), params(p), player_id(player) {}

    friend std::ostream& operator<<(std::ostream& os, const Command& cmd) {
        os << "Command(timestamp=" << cmd.timestamp;
        os << ", piece_id=" << cmd.piece_id ;
        os << ", type=" << cmd.type ;
        os << ", params_size=" << cmd.params.size();

        for (const auto& param : cmd.params) {
            os << ", {" << param.first << ":" << param.second << "}";
        }
        
        os << ")";
        return os;
    }
};
#include "CaptureRules.hpp"
#include "Piece.hpp"
#include "State.hpp"
#include <memory>

void CaptureRules::print_collision_summary(const std::pair<int,int>& cell, const std::vector<PiecePtr>& pieces) {
    std::cout << "=== COLLISION DETECTED AT CELL (" << cell.first << "," << cell.second << ") ===" << std::endl;
    std::cout << "PIECES IN COLLISION:" << std::endl;
    for (size_t k = 0; k < pieces.size(); ++k) {
        auto p = pieces[k];
        std::cout << "  [" << k << "] " << p->id << " - state: " << p->state->name 
                  << ", start_ms: " << p->state->physics->start_ms << std::endl;
    }
}

bool CaptureRules::are_same_team(PiecePtr piece1, PiecePtr piece2) {
    if (!piece1 || !piece2 || piece1->id.length() < 2 || piece2->id.length() < 2) {
        return false;
    }
    return piece1->id[1] == piece2->id[1]; // W או B
}

void CaptureRules::print_piece_analysis(PiecePtr piece, int current_game_time) {
    std::cout << piece->id << ":" << std::endl;
    std::cout << "  Current state: " << piece->state->name << std::endl;
    std::cout << "  Physics start_ms: " << piece->state->physics->start_ms << std::endl;
    std::cout << "  Physics type: " << typeid(*(piece->state->physics)).name() << std::endl;
    
    if (piece->state->name == "move") {
        auto move_phys = std::dynamic_pointer_cast<MovePhysics>(piece->state->physics);
        if (move_phys) {
            std::cout << "  Move duration: " << move_phys->get_duration_s() << "s" << std::endl;
            std::cout << "  Estimated arrival: " << (piece->state->physics->start_ms + (int)(move_phys->get_duration_s() * 1000)) << "ms" << std::endl;
        }
    }
}

int CaptureRules::calculate_arrival_time(PiecePtr piece) {
    int arrival_time = piece->state->physics->start_ms;
    
    if (piece->state->name == "move") {
        auto move_physics = std::dynamic_pointer_cast<MovePhysics>(piece->state->physics);
        if (move_physics) {
            arrival_time += (int)(move_physics->get_duration_s() * 1000);
        }
    }
    
    return arrival_time;
}

std::pair<PiecePtr, PiecePtr> CaptureRules::determine_attacker_and_victim(PiecePtr piece1, PiecePtr piece2) {
    int piece1_arrival = calculate_arrival_time(piece1);
    int piece2_arrival = calculate_arrival_time(piece2);
    
    std::cout << "ANALYSIS: " << piece1->id << " (arrival_time=" << piece1_arrival << "ms) vs " 
              << piece2->id << " (arrival_time=" << piece2_arrival << "ms)" << std::endl;
    
    // מי שמגיע אחרון הוא התוקף
    if (piece1_arrival > piece2_arrival) {
        std::cout << "ATTACKER: " << piece1->id << " (הגיע " << piece1_arrival 
                  << "ms) לוכד VICTIM: " << piece2->id << " (היה במקום מ-" << piece2_arrival << "ms)" << std::endl;
        return {piece1, piece2}; // {attacker, victim}
    } 
    else if (piece2_arrival > piece1_arrival) {
        std::cout << "ATTACKER: " << piece2->id << " (הגיע " << piece2_arrival 
                  << "ms) לוכד VICTIM: " << piece1->id << " (היה במקום מ-" << piece1_arrival << "ms)" << std::endl;
        return {piece2, piece1}; // {attacker, victim}
    } 
    else {
        // הגיעו באותו זמן - בדוק זמן התחלה
        return resolve_simultaneous_arrival(piece1, piece2, piece1_arrival);
    }
}

std::pair<PiecePtr, PiecePtr> CaptureRules::resolve_simultaneous_arrival(PiecePtr piece1, PiecePtr piece2, int arrival_time) {
    std::cout << "SIMULTANEOUS ARRIVAL: " << piece1->id << " and " << piece2->id 
              << " both arrived at " << arrival_time << "ms" << std::endl;
    std::cout << "CHECKING START TIMES: " << piece1->id << " started at " 
              << piece1->state->physics->start_ms << "ms vs " << piece2->id 
              << " started at " << piece2->state->physics->start_ms << "ms" << std::endl;
    
    if (piece1->state->physics->start_ms < piece2->state->physics->start_ms) {
        std::cout << "ATTACKER: " << piece1->id << " (יצא קודם)" << std::endl;
        return {piece1, piece2};
    } 
    else if (piece2->state->physics->start_ms < piece1->state->physics->start_ms) {
        std::cout << "ATTACKER: " << piece2->id << " (יצא קודם)" << std::endl;
        return {piece2, piece1};
    } 
    else {
        std::cout << "PERFECT TIE: Both pieces started and arrived at exactly the same time - NO CAPTURE" << std::endl;
        return {nullptr, nullptr}; // אין לכידה
    }
}

bool CaptureRules::process_collision_pair(PiecePtr piece1, PiecePtr piece2, 
                                               std::set<std::string>& already_captured,
                                               std::set<std::pair<std::string, std::string>>& reported_collisions,
                                               int current_game_time,
                                               std::function<void(PiecePtr, PiecePtr)> capture_callback) {
    // בדוק אם כבר נלכדו
    if (already_captured.count(piece1->id) || already_captured.count(piece2->id)) {
        return false;
    }
    
    // בדוק אם כבר דווח על הcollision הזה
    std::string key1 = piece1->id + "+" + piece2->id;
    std::string key2 = piece2->id + "+" + piece1->id;
    
    if (reported_collisions.find({key1, key2}) == reported_collisions.end() && 
        reported_collisions.find({key2, key1}) == reported_collisions.end()) {
        char team1 = piece1->id[1];
        char team2 = piece2->id[1];
        std::cout << "COLLISION: " << piece1->id << " (team " << team1 
                  << ") vs " << piece2->id << " (team " << team2 << ")" << std::endl;
        reported_collisions.insert({key1, key2});
    }
    
    // בדוק אם הם מאותו צוות
    if (are_same_team(piece1, piece2)) {
        std::cout << "SAME TEAM BLOCKED: " << piece1->id << " and " << piece2->id << " - NO ACTION" << std::endl;
        return false;
    }
    
    // ניתוח מפורט ותוקף/קורבן
    std::cout << "\n--- DETAILED PIECE ANALYSIS ---" << std::endl;
    print_piece_analysis(piece1, current_game_time);
    print_piece_analysis(piece2, current_game_time);
    std::cout << "Current game time: " << current_game_time << "ms" << std::endl;
    std::cout << "--- END ANALYSIS ---\n" << std::endl;
    
    auto [attacker, victim] = determine_attacker_and_victim(piece1, piece2);
    
    if (attacker && victim) {
        std::cout << "COMBAT: " << attacker->id << " fights " << victim->id << std::endl;
        std::cout << "COMBAT RESULT: " << attacker->id << " wins and captures " << victim->id << std::endl;
        already_captured.insert(victim->id);
        capture_callback(victim, attacker);
        return true; // לכידה בוצעה
    }
    
    return false;
}

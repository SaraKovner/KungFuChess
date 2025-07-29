#include <iostream>
#include <fstream>
#include "src/nlohmann/json.hpp"

int main() {
    std::ifstream f("pieces/PW/states/move/config.json");
    if (!f.is_open()) {
        std::cout << "Cannot open config file!" << std::endl;
        return 1;
    }
    
    nlohmann::json cfg;
    f >> cfg;
    
    std::cout << "Config loaded: " << cfg.dump(2) << std::endl;
    
    if (cfg.contains("physics")) {
        auto phys_cfg = cfg["physics"];
        std::cout << "Physics config: " << phys_cfg.dump(2) << std::endl;
        
        if (phys_cfg.contains("speed_m_per_sec")) {
            double speed = phys_cfg["speed_m_per_sec"];
            std::cout << "Speed: " << speed << std::endl;
        }
    }
    
    return 0;
}
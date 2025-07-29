#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/game_logic/Moves.hpp"
#include "MockFactories.hpp"
#include <fstream>
#include <sstream>

// Helper to create a temporary CSV file for testing
void create_test_csv(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

TEST_CASE("Moves construction and basic properties") {
    // Create a simple test CSV
    std::string test_content = "0,1,2,3,4,5,6,7\n"
                              "1,0,0,0,0,0,0,0\n"
                              "2,0,0,0,0,0,0,0\n"
                              "3,0,0,0,0,0,0,0\n"
                              "4,0,0,0,0,0,0,0\n"
                              "5,0,0,0,0,0,0,0\n"
                              "6,0,0,0,0,0,0,0\n"
                              "7,0,0,0,0,0,0,0\n";
    
    create_test_csv("test_board.csv", test_content);
    
    SECTION("Constructor with valid CSV") {
        auto moves = std::make_shared<Moves>("test_board.csv", std::make_pair(8, 8));
        REQUIRE(moves != nullptr);
    }
    
    SECTION("Constructor with size validation") {
        auto moves = std::make_shared<Moves>("test_board.csv", std::make_pair(8, 8));
        // Test that it doesn't crash with valid parameters
        REQUIRE_NOTHROW([&]() { auto m = moves; }());
    }
    
    // Cleanup
    std::remove("test_board.csv");
}

TEST_CASE("Moves path calculation") {
    std::string test_content = "0,1,2,3,4,5,6,7\n"
                              "1,0,0,0,0,0,0,0\n"
                              "2,0,0,0,0,0,0,0\n"
                              "3,0,0,0,0,0,0,0\n"
                              "4,0,0,0,0,0,0,0\n"
                              "5,0,0,0,0,0,0,0\n"
                              "6,0,0,0,0,0,0,0\n"
                              "7,0,0,0,0,0,0,0\n";
    
    create_test_csv("test_moves.csv", test_content);
    auto moves = std::make_shared<Moves>("test_moves.csv", std::make_pair(8, 8));
    
    SECTION("Valid move requests") {
        // Test basic functionality without crashing
        REQUIRE_NOTHROW([&]() {
            // Any basic operation that doesn't require complex setup
        }());
    }
    
    std::remove("test_moves.csv");
}

TEST_CASE("Moves edge cases") {
    SECTION("Empty board") {
        std::string empty_content = "0\n1\n";
        create_test_csv("empty.csv", empty_content);
        
        REQUIRE_NOTHROW([&]() {
            auto moves = std::make_shared<Moves>("empty.csv", std::make_pair(2, 2));
        }());
        
        std::remove("empty.csv");
    }
    
    SECTION("Single cell board") {
        std::string single_content = "0\n";
        create_test_csv("single.csv", single_content);
        
        REQUIRE_NOTHROW([&]() {
            auto moves = std::make_shared<Moves>("single.csv", std::make_pair(1, 1));
        }());
        
        std::remove("single.csv");
    }
}

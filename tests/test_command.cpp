#include "catch.hpp"
#include "../src/ui/Command.hpp"
#include <sstream>


TEST_CASE("Command construction and basic properties") {
    SECTION("Basic command creation") {
        Command cmd(1000, "P1", "move", {{2, 3}, {4, 5}}, 1);
        REQUIRE(cmd.timestamp == 1000);
        REQUIRE(cmd.piece_id == "P1");
        REQUIRE(cmd.type == "move");
        REQUIRE(cmd.params.size() == 2);
        REQUIRE(cmd.params[0] == std::make_pair(2, 3));
        REQUIRE(cmd.params[1] == std::make_pair(4, 5));
        REQUIRE(cmd.player_id == 1);
    }
    SECTION("Command with default player") {
        Command cmd(500, "Q1", "jump", {{1, 1}});
        REQUIRE(cmd.player_id == 1);
    }
    SECTION("Command with empty piece_id") {
        Command cmd(0, "", "done", {});
        REQUIRE(cmd.piece_id.empty());
        REQUIRE(cmd.params.empty());
    }
}
TEST_CASE("Command output stream operator") {
    SECTION("Command with parameters") {
        Command cmd(1500, "R1", "move", {{0, 0}, {7, 7}}, 2);
        std::ostringstream oss;
        oss << cmd;
        std::string output = oss.str();
        REQUIRE(output.find("timestamp=1500") != std::string::npos);
        REQUIRE(output.find("piece_id=R1") != std::string::npos);
        REQUIRE(output.find("type=move") != std::string::npos);
        REQUIRE(output.find("params_size=2") != std::string::npos);
        REQUIRE(output.find("{0:0}") != std::string::npos);
        REQUIRE(output.find("{7:7}") != std::string::npos);
    }
    SECTION("Command with no parameters") {
        Command cmd(100, "K1", "done", {});
        std::ostringstream oss;
        oss << cmd;
        std::string output = oss.str();
        REQUIRE(output.find("params_size=0") != std::string::npos);
    }
}
TEST_CASE("Command parameter handling") {
    SECTION("Multiple parameters") {
        std::vector<std::pair<int,int>> positions = {{1, 2}, {3, 4}, {5, 6}};
        Command cmd(2000, "N1", "path", positions);
        REQUIRE(cmd.params.size() == 3);
        for (size_t i = 0; i < positions.size(); ++i) {
            REQUIRE(cmd.params[i] == positions[i]);
        }
    }
    SECTION("Single parameter") {
        Command cmd(750, "B1", "select", {{4, 4}});
        REQUIRE(cmd.params.size() == 1);
        REQUIRE(cmd.params[0].first == 4);
        REQUIRE(cmd.params[0].second == 4);
    }
}
TEST_CASE("Command player identification") {
    SECTION("Player 1") {
        Command cmd(300, "P1", "move", {{2, 2}}, 1);
        REQUIRE(cmd.player_id == 1);
    }
    SECTION("Player 2") {
        Command cmd(300, "P2", "move", {{2, 2}}, 2);
        REQUIRE(cmd.player_id == 2);
    }
}

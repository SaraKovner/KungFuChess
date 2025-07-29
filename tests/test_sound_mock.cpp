#include "catch.hpp"
#include "../observer/headers/SoundManager.hpp"
#include <filesystem>

// Mock Sound System Tests
class MockSoundManager : public SoundManager {
public:
    mutable std::vector<std::string> playedSounds;
    mutable int victoryCallCount = 0;
    
    void playWavFile(const std::string& filename) const override {
        playedSounds.push_back(filename);
    }
    
    void playVictorySound() const override {
        victoryCallCount++;
        playedSounds.push_back("victory.wav");
    }
};

TEST_CASE("SoundManager - Mock Implementation") {
    MockSoundManager mockSound;
    
    SECTION("Track played sounds") {
        REQUIRE(mockSound.playedSounds.empty());
        REQUIRE(mockSound.victoryCallCount == 0);
        
        mockSound.playWavFile("test.wav");
        mockSound.playVictorySound();
        
        REQUIRE(mockSound.playedSounds.size() == 2);
        REQUIRE(mockSound.playedSounds[0] == "test.wav");
        REQUIRE(mockSound.playedSounds[1] == "victory.wav");
        REQUIRE(mockSound.victoryCallCount == 1);
    }
    
    SECTION("Capture event sound mapping") {
        CaptureEvent whiteCapture("PW1", "PB1", "e5", true, 1, 1000);
        CaptureEvent blackCapture("PB1", "PW1", "e4", false, 1, 2000);
        
        mockSound.onNotify(whiteCapture);
        mockSound.onNotify(blackCapture);
        
        REQUIRE(mockSound.playedSounds.size() == 2);
        REQUIRE(mockSound.playedSounds[0] == "white_capture.wav");
        REQUIRE(mockSound.playedSounds[1] == "black_capture.wav");
    }
    
    SECTION("Game state event sound mapping") {
        GameStateEvent whiteWin(GameState::WhiteWin, GameState::Playing, "White wins!", 10000);
        GameStateEvent blackWin(GameState::BlackWin, GameState::Playing, "Black wins!", 15000);
        GameStateEvent draw(GameState::Draw, GameState::Playing, "Draw!", 20000);
        
        mockSound.onNotify(whiteWin);
        mockSound.onNotify(blackWin);
        mockSound.onNotify(draw);
        
        REQUIRE(mockSound.victoryCallCount == 3);
        REQUIRE(mockSound.playedSounds.size() == 3);
    }
    
    SECTION("Non-victory game states") {
        GameStateEvent start(GameState::Playing, GameState::Paused, "Started", 0);
        GameStateEvent pause(GameState::Paused, GameState::Playing, "Paused", 5000);
        
        mockSound.onNotify(start);
        mockSound.onNotify(pause);
        
        REQUIRE(mockSound.victoryCallCount == 0);
        REQUIRE(mockSound.playedSounds.empty());
    }
}

TEST_CASE("SoundManager - File Path Handling") {
    SoundManager soundMgr;
    
    SECTION("Sound file paths") {
        // Test that the sound manager handles different file scenarios
        REQUIRE_NOTHROW([&]() {
            // These will use the default Beep if files don't exist
            soundMgr.playWavFile("white_capture.wav");
            soundMgr.playWavFile("black_capture.wav");
            soundMgr.playWavFile("victory.wav");
        }());
    }
    
    SECTION("Invalid file paths") {
        REQUIRE_NOTHROW([&]() {
            soundMgr.playWavFile("nonexistent.wav");
            soundMgr.playWavFile("");
            soundMgr.playWavFile("invalid/path/sound.wav");
        }());
    }
    
    SECTION("Sound path configuration") {
        // Test that the sound path is correctly set
        SoundManager mgr;
        // The constructor should set soundsPath_ to "../assets/sounds/"
        // This is tested indirectly through file loading
        REQUIRE_NOTHROW([&]() {
            mgr.playVictorySound();
        }());
    }
}

TEST_CASE("SoundManager - Integration with Observer Pattern") {
    MockSoundManager mockSound;
    
    SECTION("Multiple rapid events") {
        // Test handling of rapid successive events
        for(int i = 0; i < 10; i++) {
            CaptureEvent capture("PW" + std::to_string(i), "PB" + std::to_string(i), 
                               "e" + std::to_string(i), i % 2 == 0, 1, i * 1000);
            mockSound.onNotify(capture);
        }
        
        REQUIRE(mockSound.playedSounds.size() == 10);
        
        // Check alternating white/black captures
        for(int i = 0; i < 10; i++) {
            std::string expected = (i % 2 == 0) ? "white_capture.wav" : "black_capture.wav";
            REQUIRE(mockSound.playedSounds[i] == expected);
        }
    }
    
    SECTION("Mixed event types") {
        MoveEvent move("PW1", "e2", "e4", true, 1000);
        CaptureEvent capture("QW1", "QB1", "d4", true, 9, 2000);
        GameStateEvent win(GameState::WhiteWin, GameState::Playing, "Win!", 3000);
        
        // Move events don't trigger sounds in SoundManager
        mockSound.onNotify(move);
        REQUIRE(mockSound.playedSounds.empty());
        
        // Capture events do
        mockSound.onNotify(capture);
        REQUIRE(mockSound.playedSounds.size() == 1);
        REQUIRE(mockSound.playedSounds[0] == "white_capture.wav");
        
        // Win events do
        mockSound.onNotify(win);
        REQUIRE(mockSound.playedSounds.size() == 2);
        REQUIRE(mockSound.playedSounds[1] == "victory.wav");
        REQUIRE(mockSound.victoryCallCount == 1);
    }
}

TEST_CASE("SoundManager - Error Handling") {
    SECTION("Graceful handling of missing files") {
        SoundManager soundMgr;
        
        // These should not crash even if files don't exist
        REQUIRE_NOTHROW([&]() {
            soundMgr.playWavFile("definitely_does_not_exist.wav");
            soundMgr.playWavFile("../nonexistent/path/sound.wav");
            soundMgr.playWavFile("file_with_no_extension");
        }());
    }
    
    SECTION("Handle empty/null parameters") {
        SoundManager soundMgr;
        
        REQUIRE_NOTHROW([&]() {
            soundMgr.playWavFile("");
            soundMgr.playWavFile("   "); // Whitespace only
        }());
    }
}
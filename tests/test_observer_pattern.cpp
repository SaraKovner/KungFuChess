#include "catch.hpp"
#include "../observer/headers/GameEventManager.hpp"
#include "../observer/headers/WhiteMovesTable.hpp"
#include "../observer/headers/BlackMovesTable.hpp"
#include "../observer/headers/WhiteScoreTracker.hpp"
#include "../observer/headers/BlackScoreTracker.hpp"
#include "../observer/headers/VoiceAnnouncer.hpp"
#include "../observer/headers/SoundManager.hpp"

TEST_CASE("GameEventManager - Full Coverage") {
    GameEventManager manager;
    WhiteMovesTable whiteMoves;
    BlackMovesTable blackMoves;
    WhiteScoreTracker whiteScore;
    BlackScoreTracker blackScore;
    VoiceAnnouncer announcer;
    SoundManager soundMgr;
    
    SECTION("Subscribe to moves") {
        REQUIRE_NOTHROW([&]() {
            manager.subscribe(&whiteMoves, "move");
            manager.subscribe(&blackMoves, "move");
            manager.subscribe(&announcer, "move");
        }());
    }
    
    SECTION("Subscribe to captures") {
        REQUIRE_NOTHROW([&]() {
            manager.subscribe(&whiteScore, "capture");
            manager.subscribe(&blackScore, "capture");
            manager.subscribe(&announcer, "capture");
            manager.subscribe(&soundMgr, "capture");
        }());
    }
    
    SECTION("Subscribe to game state") {
        REQUIRE_NOTHROW([&]() {
            manager.subscribe(&announcer, "gamestate");
            manager.subscribe(&soundMgr, "gamestate");
        }());
    }
    
    SECTION("Publish move events") {
        manager.subscribe(&whiteMoves, "move");
        manager.subscribe(&blackMoves, "move");
        
        REQUIRE_NOTHROW([&]() {
            MoveEvent whiteMove("PW1", "e2", "e4", true, 1000);
            MoveEvent blackMove("PB1", "e7", "e5", false, 2000);
            
            manager.publish(whiteMove, "move");
            manager.publish(blackMove, "move");
        }());
    }
    
    SECTION("Publish capture events") {
        manager.subscribe(&whiteScore, "capture");
        manager.subscribe(&blackScore, "capture");
        
        REQUIRE_NOTHROW([&]() {
            CaptureEvent capture("QW1", "QB1", "d4", true, 9, 3000);
            manager.publish(capture, "capture");
        }());
    }
    
    SECTION("Publish game state events") {
        manager.subscribe(&announcer, "gamestate");
        
        REQUIRE_NOTHROW([&]() {
            GameStateEvent startEvent(GameState::Playing, GameState::Paused, "Game started", 0);
            GameStateEvent winEvent(GameState::WhiteWin, GameState::Playing, "White wins!", 10000);
            
            manager.publish(startEvent, "gamestate");
            manager.publish(winEvent, "gamestate");
        }());
    }
}

TEST_CASE("MovesTable - Full Coverage") {
    SECTION("WhiteMovesTable") {
        WhiteMovesTable whiteMoves;
        
        REQUIRE_NOTHROW([&]() {
            MoveEvent move1("PW1", "e2", "e4", true, 1000);
            MoveEvent move2("NW1", "g1", "f3", true, 2000);
            MoveEvent move3("QW1", "d1", "h5", true, 3000);
            
            whiteMoves.onNotify(move1);
            whiteMoves.onNotify(move2);
            whiteMoves.onNotify(move3);
            
            auto moves = whiteMoves.getAllMoves();
            REQUIRE(moves.size() == 3);
            REQUIRE(moves[0].piece == "PW1");
            REQUIRE(moves[1].piece == "NW1");
            REQUIRE(moves[2].piece == "QW1");
        }());
    }
    
    SECTION("BlackMovesTable") {
        BlackMovesTable blackMoves;
        
        REQUIRE_NOTHROW([&]() {
            MoveEvent move1("PB1", "e7", "e5", false, 1500);
            MoveEvent move2("NB1", "b8", "c6", false, 2500);
            
            blackMoves.onNotify(move1);
            blackMoves.onNotify(move2);
            
            auto moves = blackMoves.getAllMoves();
            REQUIRE(moves.size() == 2);
            REQUIRE(moves[0].piece == "PB1");
            REQUIRE(moves[1].piece == "NB1");
        }());
    }
    
    SECTION("Mixed color moves") {
        WhiteMovesTable whiteMoves;
        BlackMovesTable blackMoves;
        
        // White moves should only go to white table
        MoveEvent whiteMove("PW1", "e2", "e4", true, 1000);
        MoveEvent blackMove("PB1", "e7", "e5", false, 2000);
        
        whiteMoves.onNotify(whiteMove);
        whiteMoves.onNotify(blackMove); // Should be ignored
        
        blackMoves.onNotify(blackMove);
        blackMoves.onNotify(whiteMove); // Should be ignored
        
        REQUIRE(whiteMoves.getAllMoves().size() == 1);
        REQUIRE(blackMoves.getAllMoves().size() == 1);
    }
}

TEST_CASE("ScoreTracker - Full Coverage") {
    SECTION("WhiteScoreTracker") {
        WhiteScoreTracker whiteScore;
        
        REQUIRE(whiteScore.getScore() == 0);
        
        REQUIRE_NOTHROW([&]() {
            CaptureEvent capture1("PW1", "PB1", "e5", true, 1, 1000);
            CaptureEvent capture2("QW1", "RB1", "a8", true, 5, 2000);
            CaptureEvent capture3("NW1", "QB1", "d4", true, 9, 3000);
            
            whiteScore.onNotify(capture1);
            whiteScore.onNotify(capture2);
            whiteScore.onNotify(capture3);
            
            REQUIRE(whiteScore.getScore() == 15); // 1 + 5 + 9
        }());
    }
    
    SECTION("BlackScoreTracker") {
        BlackScoreTracker blackScore;
        
        REQUIRE(blackScore.getScore() == 0);
        
        REQUIRE_NOTHROW([&]() {
            CaptureEvent capture1("PB1", "PW1", "e4", false, 1, 1000);
            CaptureEvent capture2("RB1", "QW1", "d1", false, 9, 2000);
            
            blackScore.onNotify(capture1);
            blackScore.onNotify(capture2);
            
            REQUIRE(blackScore.getScore() == 10); // 1 + 9
        }());
    }
    
    SECTION("Mixed color captures") {
        WhiteScoreTracker whiteScore;
        BlackScoreTracker blackScore;
        
        CaptureEvent whiteCapture("PW1", "PB1", "e5", true, 1, 1000);
        CaptureEvent blackCapture("PB1", "PW1", "e4", false, 1, 2000);
        
        whiteScore.onNotify(whiteCapture);
        whiteScore.onNotify(blackCapture); // Should be ignored
        
        blackScore.onNotify(blackCapture);
        blackScore.onNotify(whiteCapture); // Should be ignored
        
        REQUIRE(whiteScore.getScore() == 1);
        REQUIRE(blackScore.getScore() == 1);
    }
}

TEST_CASE("VoiceAnnouncer - Full Coverage") {
    VoiceAnnouncer announcer;
    
    SECTION("Announce moves") {
        REQUIRE_NOTHROW([&]() {
            MoveEvent whiteMove("PW1", "e2", "e4", true, 1000);
            MoveEvent blackMove("PB1", "e7", "e5", false, 2000);
            
            announcer.onNotify(whiteMove);
            announcer.onNotify(blackMove);
        }());
    }
    
    SECTION("Announce captures") {
        REQUIRE_NOTHROW([&]() {
            CaptureEvent capture("QW1", "QB1", "d4", true, 9, 3000);
            announcer.onNotify(capture);
        }());
    }
    
    SECTION("Announce game state changes") {
        REQUIRE_NOTHROW([&]() {
            GameStateEvent startEvent(GameState::Playing, GameState::Paused, "Game started", 0);
            GameStateEvent winEvent(GameState::WhiteWin, GameState::Playing, "White wins!", 10000);
            GameStateEvent drawEvent(GameState::Draw, GameState::Playing, "It's a draw!", 15000);
            
            announcer.onNotify(startEvent);
            announcer.onNotify(winEvent);
            announcer.onNotify(drawEvent);
        }());
    }
}

TEST_CASE("SoundManager - Full Coverage") {
    SoundManager soundMgr;
    
    SECTION("Play capture sounds") {
        REQUIRE_NOTHROW([&]() {
            CaptureEvent whiteCapture("PW1", "PB1", "e5", true, 1, 1000);
            CaptureEvent blackCapture("PB1", "PW1", "e4", false, 1, 2000);
            
            soundMgr.onNotify(whiteCapture);
            soundMgr.onNotify(blackCapture);
        }());
    }
    
    SECTION("Play victory sounds") {
        REQUIRE_NOTHROW([&]() {
            GameStateEvent whiteWin(GameState::WhiteWin, GameState::Playing, "White wins!", 10000);
            GameStateEvent blackWin(GameState::BlackWin, GameState::Playing, "Black wins!", 15000);
            
            soundMgr.onNotify(whiteWin);
            soundMgr.onNotify(blackWin);
        }());
    }
    
    SECTION("Handle non-victory game states") {
        REQUIRE_NOTHROW([&]() {
            GameStateEvent startEvent(GameState::Playing, GameState::Paused, "Game started", 0);
            GameStateEvent pauseEvent(GameState::Paused, GameState::Playing, "Game paused", 5000);
            
            soundMgr.onNotify(startEvent);
            soundMgr.onNotify(pauseEvent);
        }());
    }
    

}

TEST_CASE("Event Objects - Full Coverage") {
    SECTION("MoveEvent construction") {
        REQUIRE_NOTHROW([&]() {
            MoveEvent move1("PW1", "e2", "e4", true, 1000);
            MoveEvent move2("", "", "", false, 0); // Empty values
            
            REQUIRE(move1.piece == "PW1");
            REQUIRE(move1.from == "e2");
            REQUIRE(move1.to == "e4");
            REQUIRE(move1.isWhite == true);
            REQUIRE(move1.timestamp == 1000);
        }());
    }
    
    SECTION("CaptureEvent construction") {
        REQUIRE_NOTHROW([&]() {
            CaptureEvent capture1("QW1", "QB1", "d4", true, 9, 3000);
            CaptureEvent capture2("", "", "", false, 0, 0); // Empty values
            
            REQUIRE(capture1.capturingPiece == "QW1");
            REQUIRE(capture1.capturedPiece == "QB1");
            REQUIRE(capture1.position == "d4");
            REQUIRE(capture1.capturerIsWhite == true);
            REQUIRE(capture1.scoreValue == 9);
            REQUIRE(capture1.timestamp == 3000);
        }());
    }
    
    SECTION("GameStateEvent construction") {
        REQUIRE_NOTHROW([&]() {
            GameStateEvent event1(GameState::Playing, GameState::Paused, "Started", 1000);
            GameStateEvent event2(GameState::WhiteWin, GameState::Playing, "", 0); // Empty reason
            
            REQUIRE(event1.newState == GameState::Playing);
            REQUIRE(event1.previousState == GameState::Paused);
            REQUIRE(event1.reason == "Started");
            REQUIRE(event1.timestamp == 1000);
        }());
    }
}
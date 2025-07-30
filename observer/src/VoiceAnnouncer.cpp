#include "../headers/VoiceAnnouncer.hpp"
#include <iostream>

std::string VoiceAnnouncer::getPieceNameEnglish(const std::string& piece) const {
    if (piece == "P") return "Pawn";
    if (piece == "R") return "Rook";
    if (piece == "N") return "Knight";
    if (piece == "B") return "Bishop";
    if (piece == "Q") return "Queen";
    if (piece == "K") return "King";
    return piece;
}

std::string VoiceAnnouncer::getPlayerName(bool isWhite) const {
    return isWhite ? "White" : "Black";
}

void VoiceAnnouncer::onNotify(const BaseEvent& event) {
    if (auto* moveEvent = dynamic_cast<const MoveEvent*>(&event)) {
        announceMove(*moveEvent);
    }
    else if (auto* captureEvent = dynamic_cast<const CaptureEvent*>(&event)) {
        announceCapture(*captureEvent);
    }
    else if (auto* gameEvent = dynamic_cast<const GameStateEvent*>(&event)) {
        announceGameState(*gameEvent);
    }
}

void VoiceAnnouncer::announceMove(const MoveEvent& event) {
    std::string player = getPlayerName(event.isWhite);
    std::string piece = getPieceNameEnglish(event.piece);
    
    std::cout << "Move: " << player << " moved " << piece 
              << " from " << event.from << " to " << event.to << std::endl;
}

void VoiceAnnouncer::announceCapture(const CaptureEvent& event) {
    std::string capturer = getPlayerName(event.capturerIsWhite);
    std::string capturingPiece = getPieceNameEnglish(event.capturingPiece);
    std::string capturedPiece = getPieceNameEnglish(event.capturedPiece);
    
    std::cout << "Capture: " << capturer << " captured!" << std::endl;
    std::cout << "   " << capturingPiece << " captured " << capturedPiece 
              << " at " << event.position << std::endl;
              
    // Special messages by piece value
    if (event.capturedPiece == "Q") {
        std::cout << "Crown: Queen captured! This is a significant move!" << std::endl;
    } else if (event.capturedPiece == "R") {
        std::cout << "Castle: Rook captured! Strategy is changing!" << std::endl;
    }
}

void VoiceAnnouncer::announceGameState(const GameStateEvent& event) {
    switch (event.newState) {
        case GameState::WhiteWin:
            std::cout << "Victory: White wins! Congratulations!" << std::endl;
            break;
        case GameState::BlackWin:
            std::cout << "Victory: Black wins! Congratulations!" << std::endl;
            break;
        case GameState::Draw:
            std::cout << "Draw: Game ended in a draw!" << std::endl;
            break;
        case GameState::Paused:
            std::cout << "Pause: Game paused" << std::endl;
            break;
        case GameState::Playing:
            std::cout << "Resume: Game resumed!" << std::endl;
            break;
    }
    
    if (!event.reason.empty()) {
        std::cout << "   Reason: " << event.reason << std::endl;
    }
}

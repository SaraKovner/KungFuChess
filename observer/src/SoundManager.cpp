#include "../headers/SoundManager.hpp"
#include <iostream>
#include <filesystem>

SoundManager::SoundManager() {
    soundsPath_ = "sounds/";
}

void SoundManager::onNotify(const CaptureEvent& event) {
    std::cout << "🔊 Playing capture sound for: " << event.capturingPiece 
              << " captures " << event.capturedPiece << std::endl;
    
    if (event.capturerIsWhite) {
        playWavFile("white_capture.wav");
    } else {
        playWavFile("black_capture.wav");
    }
}

void SoundManager::onNotify(const GameStateEvent& event) {
    if (event.newState == GameState::WhiteWin || event.newState == GameState::BlackWin) {
        std::cout << "🎉 Playing victory sound!" << std::endl;
        playVictorySound();
    }
}

void SoundManager::playVictorySound() {
    playWavFile("victory.wav");
}

void SoundManager::playWavFile(const std::string& filename) {
    std::string fullPath = soundsPath_ + filename;
    
    if (!std::filesystem::exists(fullPath)) {
        std::cout << "Sound file not found: " << fullPath << std::endl;
        Beep(800, 200);
        return;
    }
    
    std::wstring wPath(fullPath.begin(), fullPath.end());
    PlaySoundW(wPath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}
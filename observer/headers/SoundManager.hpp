#pragma once
#include "Observer.hpp"
#include "CaptureEvent.hpp"
#include "GameStateEvent.hpp"
#include <windows.h>
#include <string>

class SoundManager : public Observer<CaptureEvent>, public Observer<GameStateEvent> {
public:
    SoundManager();
    
    void onNotify(const CaptureEvent& event) override;
    void onNotify(const GameStateEvent& event) override;

private:
    void playVictorySound();
    void playWavFile(const std::string& filename);
    
    std::string soundsPath_;
};
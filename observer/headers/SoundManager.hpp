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
    
    // Made public for testing
    virtual void playVictorySound();
    virtual void playWavFile(const std::string& filename);

private:
    
    std::string soundsPath_;
};
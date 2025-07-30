#pragma once
#include "Observer.hpp"
#include "CaptureEvent.hpp"
#include "GameStateEvent.hpp"
#include "BaseEvent.hpp"
#include <windows.h>
#include <string>

class SoundManager : public Observer {
public:
    SoundManager();
    
    void onNotify(const BaseEvent& event) override;
    
    // Made public for testing
    virtual void playVictorySound();
    virtual void playWavFile(const std::string& filename);

private:
    void handleCapture(const CaptureEvent& event);
    void handleGameState(const GameStateEvent& event);
    std::string soundsPath_;
};
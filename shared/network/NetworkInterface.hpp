#pragma once
#include <string>

// Interface for sending game moves to network
class NetworkInterface {
public:
    virtual ~NetworkInterface() = default;
    virtual void sendMove(const std::string& move) = 0;
    virtual void onMoveReceived(const std::string& move) = 0;
};
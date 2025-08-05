#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "../../shared/network/NetworkInterface.hpp"

// הכרזה מוקדמת
class Game;

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

// צבעי שחקנים עם מצב לא ידוע
enum class PlayerColor { WHITE, BLACK, UNKNOWN };

/**
 * מחלקת לקוח המשחק - מתחברת לשרת ומנהלת את התקשורת
 * מימשת את NetworkInterface לתקשורת רשת
 */
class GameClient : public NetworkInterface {
private:
    int socket_fd_;                          // מזהה socket של החיבור
    std::string server_address_;             // כתובת השרת
    int server_port_;                        // פורט השרת
    std::atomic<bool> connected_{false};     // האם מחובר לשרת
    PlayerColor my_color_{PlayerColor::UNKNOWN}; // צבע השחקן הנוכחי
    Game* game_{nullptr};                    // הפניה למופע המשחק
    
    // פונקציית עזר לקבלת מזהה שחקן לפי צבע
    int getMyPlayerId() const {
        return (my_color_ == PlayerColor::WHITE) ? 1 : 2;
    }
    
public:
    // בנאי והרס
    GameClient(const std::string& server_address, int port = 8080); // בנאי עם כתובת שרת ופורט
    ~GameClient();                                                  // הרס - מנקה משאבים
    
    // פונקציות חיבור וניהול
    bool connect();    // חיבור לשרת
    void run();        // לולאת ראשית של הלקוח
    void disconnect(); // ניתוק מהשרת
    
    // הגדרת מופע המשחק לסינכרון רשת
    void setGame(Game* game);
    
    // מימוש NetworkInterface
    void sendMove(const std::string& move) override;        // שליחת תנועה לשרת
    void onMoveReceived(const std::string& move) override;  // טיפול בתנועה שהתקבלה
    
private:
    // פונקציות תקשורת
    void receiveMessages();                                 // קבלת הודעות מהשרת
    void sendMessage(const std::string& message);          // שליחת הודעה לשרת
    void handleMessage(const std::string& message);        // טיפול בהודעה שהתקבלה
    void handleServerCommand(const std::string& command);  // טיפול בפקודות שרת
    void simulateGameplay();                               // סימולציה של משחק
    
    // פונקציות Winsock (Windows)
    void initializeWinsock(); // איניציאליזציה
    void cleanupWinsock();    // ניקוי
};

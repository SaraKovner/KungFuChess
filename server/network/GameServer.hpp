#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <queue>
#include <memory>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

// הכללות מנוע המשחק
#include "../../shared/core/Game.hpp"
#include "../../shared/core/Board.hpp"
#include "../../shared/game_logic/PieceFactory.hpp"
#include "../../shared/graphics/img/ImgFactory.hpp"
#include "../../shared/graphics/img/MockImg.hpp"
#include "../game_logic/ServerGameEngine.hpp"

// צבעי שחקנים
enum class PlayerColor { WHITE, BLACK };

/**
 * מבנה חיבור שחקן - מידע על שחקן מחובר
 */
struct PlayerConnection {
    int socket_fd;        // מזהה socket של החיבור
    PlayerColor color;    // צבע השחקן (לבן/שחור)
    bool connected;       // האם מחובר
    std::string name;     // שם השחקן
};

/**
 * מחלקת שרת המשחק - מנהלת את כל התקשורת עם הלקוחות
 * אחראית על לוגיקת המשחק ומסנכרנת בין השחקנים
 */
class GameServer {
private:
    int server_socket_;                      // socket ראשי של השרת
    int port_;                               // פורט השרת
    std::atomic<bool> running_{false};       // האם השרת פועל
    std::vector<PlayerConnection> players_;  // רשימת שחקנים מחוברים
    std::mutex players_mutex_;               // נעילה לרשימת שחקנים
    
    // תור פקודות מכל הלקוחות
    std::queue<std::string> command_queue_;
    std::mutex queue_mutex_;
    
    // מנוע המשחק - השרת הוא הסמכות העליונה
    std::unique_ptr<ServerGameEngine> server_engine_;
    std::mutex game_mutex_;
    
    // מעקב מצב שחקנים בצד השרת
    std::mutex player_state_mutex_;

public:
    // בנאי והרס
    GameServer(int port = 8080);  // בנאי עם פורט ברירת מחדל
    ~GameServer();               // הרס - מנקה משאבים
    
    // פונקציות שליטה בשרת
    bool start();  // התחלת השרת ואיניציאליזציה
    void run();    // לולאת ראשית של השרת
    void stop();   // עצירת השרת וניקוי
    
private:
    // פונקציות רשת
    void acceptClients();                                    // קבלת חיבורי לקוחות חדשים
    void handleClient(int client_socket, PlayerColor color); // טיפול בלקוח ספציפי
    void broadcastMessage(const std::string& message);      // שידור הודעה לכל הלקוחות
    void processCommands();                                  // עיבוד פקודות מהתור
    void handleInputCommand(const std::string& command);     // טיפול בקלט מלקוחות - שליחת פקודות חזרה
    
    // פונקציות לוגיקת משחק
    void initializeGame();                                           // איניציאליזציה של המשחק
    bool validateInput(int player_id, const std::string& cmd_type);  // אימות קלט שחקן
    void processValidatedInput(int player_id, const std::string& cmd_type); // עיבוד קלט מאומת
    void broadcastGameState();                                       // שידור מצב המשחק
    void checkWinCondition();                                        // בדיקת תנאי ניצחון
    
    // אימות מצב משחק בצד השרת
    bool isValidCursorMove(int player_id, const std::string& direction); // אימות תנועת סמן
    bool isValidPieceSelection(int player_id);                           // אימות בחירת כלי
    bool isValidPieceMove(int player_id);                                // אימות תנועת כלי
    bool validateSelectCommand(int player_id);                           // אימות פקודת בחירה
    
    // מצב שחקן בצד השרת
    struct PlayerState {
        std::pair<int, int> cursor_pos = {-1, -1};          // מיקום הסמן
        std::pair<int, int> selected_piece_pos = {-1, -1};  // מיקום הכלי הנבחר
        bool has_selected_piece = false;                    // האם יש כלי נבחר
    };
    PlayerState player1_state_;  // מצב שחקן 1
    PlayerState player2_state_;  // מצב שחקן 2
    
    // פונקציות עזר
    void sendToClient(int socket, const std::string& message);  // שליחת הודעה ללקוח ספציפי
    std::string receiveFromClient(int socket);                  // קבלת הודעה מלקוח
    void initializeWinsock();                                   // איניציאליזציה של Winsock (Windows)
    void cleanupWinsock();                                      // ניקוי Winsock
};

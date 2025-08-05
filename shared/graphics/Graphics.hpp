#pragma once

#include "img/ImgFactory.hpp"
#include "../ui/Command.hpp"
#include <vector>
#include <string>

/**
 * מחלקת גרפיקה - מטפלת באנימציות של כלים
 * מנהלת רצף תמונות (sprites) ומעברת ביניהן לפי FPS
 */
class Graphics {
public:
	// בנאי - מקבל תיקיית sprites, גודל תא, מפעל תמונות והגדרות אנימציה
	Graphics(const std::string& sprites_folder,  // נתיב לתיקיית sprites
		std::pair<int, int> cell_size,          // גודל תא בפיקסלים
		ImgFactoryPtr img_factory,              // מפעל ליצירת תמונות
		bool loop = true,                       // האם לחזור על האנימציה
		double fps = 0.2);                      // מספר פריימים לשנייה

	void reset(const Command& cmd);    // איפוס האנימציה לפי פקודה
	void update(int now_ms);           // עדכון הפריים הנוכחי לפי הזמן
	const ImgPtr get_img() const;      // קבלת התמונה הנוכחית

	// פונקציות עזר לבדיקות ---------------------------------------------------------
	size_t current_frame() const { return cur_frame; }                           // פריים נוכחי
	void set_frames(const std::vector<ImgPtr>& new_frames) { frames = new_frames; } // הגדרת פריימים חדשים

private:
	std::vector<ImgPtr> frames;    // רשימת פריימי האנימציה
	bool loop{ true };             // האם לחזור על האנימציה
	double fps{ 0.2 };             // מספר פריימים לשנייה
	int start_ms{ 0 };             // זמן התחלת האנימציה
	size_t cur_frame{ 0 };         // אינדקס הפריים הנוכחי
	double frame_duration_ms{ 0 }; // משך פריים במילישניות
};
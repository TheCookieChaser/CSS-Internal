#pragma once

#include "../hooks/hooks.h"

class cdrawmanager
{
public:
	void begin();
	void render();
	void end();
	void clear();

	void add_line(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
	void add_rect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f,
	              int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.0f);
	void add_filled_rect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f,
	                     int rounding_corners_flags = ImDrawCornerFlags_All);
	void add_circle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12, float thickness = 1.0f);
	void add_filled_circle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12);
	void add_text(const ImVec2& pos, ImU32 col, const char* text);

private:
	enum class draw_types_e
	{
		none,
		line,
		rect,
		filled_rect,
		circle,
		filled_circle,
		text,
	};

	class cdrawinfo
	{
	public:
		cdrawinfo():
			type(),
			thickness(0),
			rounding(0),
			flags(0),
			radius(0),
			num_segments(0)
		{
		}

		draw_types_e type;
		ImVec2 pos, pos2;
		ImColor col;
		float thickness;
		float rounding;
		int flags;
		float radius;
		int num_segments;
		std::string text;
		bool render = false;
	};

	std::vector<cdrawinfo> m_drawinfos;
};

extern cdrawmanager* drawmanager;

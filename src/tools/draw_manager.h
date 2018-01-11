#pragma once

#include "../hooks/hooks.h"
#include <array>

class cdrawmanager
{
public:
	void begin();
	void render();
	void end();
	void clear();
	void add_line(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
	void add_rect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.0f);
	void add_filled_rect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);
	void add_text(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);

private:
	enum class draw_types_e
	{
		line,
		rect,
		filled_rect,
		text,
	};

	struct draw_info_s
	{
		draw_types_e type = static_cast<draw_types_e>(0);
		ImVec2 pos{ 0,0 }, pos2{ 0,0 };
		ImColor col = ImColor(0);
		const char* text_begin = reinterpret_cast<const char*>(0);
		const char* text_end = reinterpret_cast<const char*>(0);
		float thickness = 0.f;
		float rounding = 0.f;
		int flags = 0;
		bool render = false;
	};

	std::array<draw_info_s, 512>m_draw_list{ draw_info_s{} };
	int m_counter = 0;
};

extern cdrawmanager* drawmanager;

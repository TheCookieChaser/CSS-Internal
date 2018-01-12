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
	void add_circle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12, float thickness = 1.0f);
	void add_filled_circle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12);
	void add_text(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);

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
		cdrawinfo()
		{
			type = draw_types_e::none;
			pos = { 0,0 };
			pos2 = { 0,0 };
			col = ImColor(0);
			thickness = 0.f;
			rounding = 0.f;
			flags = 0;
			radius = 0.f;
			num_segments = 0;
			text_begin = "";
			text_end = "";
			render = false;
		}

		draw_types_e type = static_cast<draw_types_e>(0);
		ImVec2 pos{ 0,0 }, pos2{ 0,0 };
		ImColor col = ImColor(0);
		float thickness = 0.f;
		float rounding = 0.f;
		int flags = 0;
		float radius;
		int num_segments;
		const char* text_begin;
		const char* text_end;
		bool render = false;
	};

	std::vector<cdrawinfo>m_drawinfos;
};

extern cdrawmanager* drawmanager;

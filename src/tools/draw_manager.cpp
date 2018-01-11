#include "draw_manager.h"
#include <imgui/imgui_internal.h>

cdrawmanager* drawmanager = new cdrawmanager();

void cdrawmanager::begin()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("BackBuffer", reinterpret_cast<bool*>(true), ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);
}

void cdrawmanager::render()
{
	auto drawlist = ImGui::GetCurrentWindow()->DrawList;
	for (size_t i = 0; i < m_draw_list.size(); i++)
	{
		auto item = m_draw_list.at(i);
		if (!item.render)
			continue;

		if (item.type == draw_types_e::line)
			drawlist->AddLine(item.pos, item.pos2, item.col, item.thickness);
		if (item.type == draw_types_e::rect)
			drawlist->AddRect(item.pos, item.pos2, item.col, item.rounding, item.flags, item.thickness);
		if (item.type == draw_types_e::filled_rect)
			drawlist->AddRectFilled(item.pos, item.pos2, item.col, item.rounding, item.flags);
		if (item.type == draw_types_e::text)
			drawlist->AddText(item.pos, item.col, item.text_begin, item.text_end);
	}
}

void cdrawmanager::end()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PushClipRectFullScreen();

	ImGui::End();
	ImGui::PopStyleColor();
}

void cdrawmanager::clear()
{
	m_draw_list.fill(draw_info_s{ });
	m_counter = 0;
}

void cdrawmanager::add_line(const ImVec2 & a, const ImVec2 & b, ImU32 col, float thickness)
{
	draw_info_s item;
	item.type = draw_types_e::line;
	item.pos = a;
	item.pos2 = b;
	item.col = col;
	item.thickness = thickness;
	item.render = true;

	m_draw_list.at(m_counter) = item;
	m_counter++;
}

void cdrawmanager::add_rect(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
	draw_info_s item;
	item.type = draw_types_e::rect;
	item.pos = a;
	item.pos2 = b;
	item.col = col;
	item.rounding = rounding;
	item.flags = rounding_corners_flags;
	item.thickness = thickness;
	item.render = true;

	m_draw_list.at(m_counter) = item;
	m_counter++;
}

void cdrawmanager::add_filled_rect(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags)
{
	draw_info_s item;
	item.type = draw_types_e::filled_rect;
	item.pos = a;
	item.pos2 = b;
	item.col = col;
	item.rounding = rounding;
	item.flags = rounding_corners_flags;
	item.render = true;

	m_draw_list.at(m_counter) = item;
	m_counter++;
}

void cdrawmanager::add_text(const ImVec2 & pos, ImU32 col, const char * text_begin, const char * text_end)
{
	draw_info_s item;
	item.type = draw_types_e::text;
	item.pos = pos;
	item.col = col;
	item.text_begin = text_begin;
	item.text_end = text_end;
	item.render = true;

	m_draw_list.at(m_counter) = item;
	m_counter++;
}

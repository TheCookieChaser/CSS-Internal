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

	for (auto drawinfo : m_drawinfos)
	{
		if (!drawinfo.render)
			continue;

		if (drawinfo.type == draw_types_e::line)
			drawlist->AddLine(drawinfo.pos, drawinfo.pos2, drawinfo.col, drawinfo.thickness);
		if (drawinfo.type == draw_types_e::rect)
			drawlist->AddRect(drawinfo.pos, drawinfo.pos2, drawinfo.col, drawinfo.rounding, drawinfo.flags, drawinfo.thickness);
		if (drawinfo.type == draw_types_e::filled_rect)
			drawlist->AddRectFilled(drawinfo.pos, drawinfo.pos2, drawinfo.col, drawinfo.rounding, drawinfo.flags);
		if (drawinfo.type == draw_types_e::circle)
			drawlist->AddCircle(drawinfo.pos, drawinfo.radius, drawinfo.col, drawinfo.num_segments, drawinfo.thickness);
		if (drawinfo.type == draw_types_e::filled_circle)
			drawlist->AddCircleFilled(drawinfo.pos, drawinfo.radius, drawinfo.col, drawinfo.num_segments);
		if (drawinfo.type == draw_types_e::text)
			drawlist->AddText(drawinfo.pos, drawinfo.col, drawinfo.text.c_str());
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
	m_drawinfos.clear();
	m_drawinfos.push_back(cdrawinfo());
}

void cdrawmanager::add_line(const ImVec2 & a, const ImVec2 & b, ImU32 col, float thickness)
{
	cdrawinfo item;
	item.type = draw_types_e::line;
	item.pos = a;
	item.pos2 = b;
	item.col = col;
	item.thickness = thickness;
	item.render = true;

	m_drawinfos.push_back(item);
}

void cdrawmanager::add_rect(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
	cdrawinfo item;
	item.type = draw_types_e::rect;
	item.pos = a;
	item.pos2 = b;
	item.col = col;
	item.rounding = rounding;
	item.flags = rounding_corners_flags;
	item.thickness = thickness;
	item.render = true;

	m_drawinfos.push_back(item);
}

void cdrawmanager::add_filled_rect(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags)
{
	cdrawinfo item;
	item.type = draw_types_e::filled_rect;
	item.pos = a;
	item.pos2 = b;
	item.col = col;
	item.rounding = rounding;
	item.flags = rounding_corners_flags;
	item.render = true;

	m_drawinfos.push_back(item);
}

void cdrawmanager::add_circle(const ImVec2 & centre, float radius, ImU32 col, int num_segments, float thickness)
{
	cdrawinfo item;
	item.type = draw_types_e::circle;
	item.pos = centre;
	item.radius = radius;
	item.col = col;
	item.num_segments = num_segments;
	item.thickness = thickness;
	item.render = true;

	m_drawinfos.push_back(item);
}

void cdrawmanager::add_filled_circle(const ImVec2 & centre, float radius, ImU32 col, int num_segments)
{
	cdrawinfo item;
	item.type = draw_types_e::filled_circle;
	item.pos = centre;
	item.radius = radius;
	item.col = col;
	item.num_segments = num_segments;
	item.render = true;

	m_drawinfos.push_back(item);
}

void cdrawmanager::add_text(const ImVec2 & pos, ImU32 col, const char * text)
{
	cdrawinfo item;
	item.type = draw_types_e::text;
	item.pos = pos;
	item.col = col;
	item.text = text;
	item.render = true;

	m_drawinfos.push_back(item);
}

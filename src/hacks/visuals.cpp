#include "visuals.h"
#include "../tools/math.h"

cvisuals* visuals = new cvisuals();

void cvisuals::render()
{
	drawmanager->add_text({ 10.f, 10.f }, config.colors_watermark, "CSS-Internal");

	render_esp();
}

void cvisuals::render_esp()
{
	if (!config.visuals_enabled)
		return;

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local)
		return;
	
	for (auto i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
	{
		auto entity = static_cast<C_BaseEntity*>(g_entitylist->GetClientEntity(i));
		if (!entity || entity->IsDormant())
			continue;

		auto clientclass = entity->GetClientClass();
		if (!clientclass)
			continue;

		//if (clientclass->m_ClassID == 35)
		{
			render_players(static_cast<C_CSPlayer*>(entity));
		}
	}
}

void cvisuals::render_players(C_CSPlayer* player)
{
	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (player->get_life_state() == 1
		|| player->get_team_num() == local->get_team_num()
		|| player == local)
		return;

	player_info_t info;
	if (!g_engine->GetPlayerInfo(player->GetIndex(), &info))
		return;

	auto weapon = player->GetActiveWeapon();
	if (!weapon)
		return;

	auto weapondata = weapon->GetWpnData();
	if (!weapondata)
		return;

	int screen_width, screen_height;
	g_engine->GetScreenSize(screen_width, screen_height);

	rect_s rect{};
	if (get_bounding_box(player, rect))
	{
		//drawmanager->add_text({ rect.left,rect.top }, ImColor(255, 255, 255, 255), "left, top");
		//drawmanager->add_text({ rect.right, rect.bottom }, ImColor(255, 255, 255, 255), "right, bottom");

		if (config.visuals_box)
		{
			drawmanager->add_rect({ rect.left, rect.top }, { rect.right, rect.bottom }, get_player_color(player));
		}

		if (config.visuals_health)
		{
			drawmanager->add_rect({ rect.left, rect.bottom + 2 },
				{ rect.left + (rect.right - rect.left) / 100.f * player->get_health(), rect.bottom + 2 },
				get_health_color(player->get_health()));
		}

		if (config.visuals_snapline)
		{
			drawmanager->add_line({ screen_width / 2, screen_height },
				{ rect.left + (rect.right - rect.left) / 2, rect.bottom }, ImColor(255, 255, 255, 255));
		}

		if (config.visuals_name)
		{
			drawmanager->add_text({ top.x, top.y - 16 }, ImColor(255, 255, 255, 255), info.szName);
		}

		if (config.visuals_weapon)
		{
			drawmanager->add_text({ top.x, top.y + height }, ImColor(255, 255, 255, 255), weapondata->weaponname);
		}
	}

	//Vector mins, maxs;
	//Vector pos, top;
	//player->GetRenderBounds(mins, maxs);
	//const auto pos3_d = player->get_origin();
	//const auto top3_d = pos3_d + Vector(0, 0, maxs.z);

	//if (g_debugoverlay->ScreenPosition(pos3_d, pos) || g_debugoverlay->ScreenPosition(top3_d, top))
	//	return;

	//const auto height = (pos.y - top.y);
	//const auto width = height / 4.f;

	//if (config.visuals_box)
	//{
	//	drawmanager->add_rect({ top.x - width, top.y }, { top.x + width, top.y + height }, get_player_color(player));
	//	drawmanager->add_rect({ top.x - width - 1, top.y - 1 }, { top.x + width + 1, top.y + height + 1 }, ImColor(0, 0, 0, 255));
	//	drawmanager->add_rect({ top.x - width + 1, top.y + 1 }, { top.x + width - 1, top.y + height - 1 }, ImColor(0, 0, 0, 255));
	//}

	//if (config.visuals_health)
	//{
	//	float pos = height - (float)((player->get_health() * height) / 100);
	//	drawmanager->add_filled_rect({ top.x - width - 6, top.y + pos }, { top.x - width - 2, top.y + height }, get_health_color(player->get_health()));
	//	drawmanager->add_rect({ top.x - width - 6 , top.y }, { top.x - width - 2, top.y + height }, ImColor(0, 0, 0, 255));
	//}

	//if (config.visuals_snapline)
	//{
	//	int screen_width, screen_height;
	//	g_engine->GetScreenSize(screen_width, screen_height);

	//	drawmanager->add_line({ screen_width / 2.f, (float)screen_height }, { top.x, top.y + height }, ImColor(255, 255, 255, 255));
	//}

	//if (config.visuals_name)
	//{
	//	drawmanager->add_text({ top.x, top.y - 16 }, ImColor(255, 255, 255, 255), info.szName);
	//}

	//if (config.visuals_weapon)
	//{
	//	drawmanager->add_text({ top.x, top.y + height }, ImColor(255, 255, 255, 255), weapondata->weaponname);
	//}
}

ImColor cvisuals::get_player_color(C_CSPlayer* pEntity)
{
	if (pEntity->get_team_num() == 2)
		return entity::is_visible(pEntity, 12) ? config.colors_esp_visible_t : config.colors_esp_t;
	else if (pEntity->get_team_num() == 3)
		return entity::is_visible(pEntity, 12) ? config.colors_esp_visible_ct : config.colors_esp_ct;

	return ImColor();
}

ImColor cvisuals::get_health_color(int health)
{
	health = max(0, min(health, 100));

	return ImColor(min((510 * (100 - health)) / 100, 255), min((510 * health) / 100, 255), 0, 255);
}

bool cvisuals::get_bounding_box(C_BasePlayer* entity, rect_s& rect)
{
	matrix3x4_t trans;
	math::AngleMatrix(entity->GetRenderAngles(), entity->GetRenderOrigin(), trans);

	Vector min, max;
	entity->GetRenderBounds(min, max);

	std::array<Vector, 8> points
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	for (auto& point : points)
	{
		Vector point_transformed;
		math::VectorTransform(point, trans, point_transformed);
		if (g_debugoverlay->ScreenPosition(point_transformed, point))
			return false;
	}

	auto left = points[0].x;
	auto top = points[0].y;
	auto right = points[0].x;
	auto bottom = points[0].y;

	for (auto i = 0; i < 8; i++)
	{
		if (left > points[i].x)
			left = points[i].x;
		if (bottom < points[i].y)
			bottom = points[i].y;
		if (right < points[i].x)
			right = points[i].x;
		if (top > points[i].y)
			top = points[i].y;
	}

	rect = { left, top, right, bottom };

	return true;
}
#include "visuals.h"

cvisuals* visuals = new cvisuals();

void cvisuals::render()
{
	drawmanager->add_text({ 10,10 }, config.colors_watermark, "CSS-Internal");

	render_esp();
}

void cvisuals::render_esp()
{
	if (!config.visuals_enabled)
		return;

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local)
		return;

	for (int i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
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

	Vector mins, maxs;
	Vector pos3D, top3D;
	Vector pos, top;
	player->GetRenderBounds(mins, maxs);
	pos3D = player->get_origin();
	top3D = pos3D + Vector(0, 0, maxs.z);

	if (g_debugoverlay->ScreenPosition(pos3D, pos) || g_debugoverlay->ScreenPosition(top3D, top))
		return;

	float height = (pos.y - top.y);
	float width = height / 4.f;

	if (config.visuals_box)
	{
		drawmanager->add_rect({ top.x - width, top.y }, { top.x + width, top.y + height }, get_player_color(player));
		drawmanager->add_rect({ top.x - width - 1, top.y - 1 }, { top.x + width + 1, top.y + height + 1 }, ImColor(0, 0, 0, 255));
		drawmanager->add_rect({ top.x - width + 1, top.y + 1 }, { top.x + width - 1, top.y + height - 1 }, ImColor(0, 0, 0, 255));
	}

	if (config.visuals_health)
	{
		float pos = height - (float)((player->get_health() * height) / 100);
		drawmanager->add_filled_rect({ top.x - width - 6, top.y + pos }, { top.x - width - 2, top.y + height }, get_health_color(player->get_health()));
		drawmanager->add_rect({ top.x - width - 6 , top.y }, { top.x - width - 2, top.y + height }, ImColor(0, 0, 0, 255));
	}

	if (config.visuals_snapline)
	{
		int screen_width, screen_height;
		g_engine->GetScreenSize(screen_width, screen_height);

		drawmanager->add_line({ screen_width / 2.f, (float)screen_height }, { top.x, top.y + height }, ImColor(255, 255, 255, 255));
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

ImColor cvisuals::get_player_color(C_CSPlayer* pEntity)
{
	if (pEntity->get_team_num() == 2)
		return Entity::IsVisible(pEntity, 12) ? config.colors_esp_visible_t : config.colors_esp_t;
	else if (pEntity->get_team_num() == 3)
		return Entity::IsVisible(pEntity, 12) ? config.colors_esp_visible_ct : config.colors_esp_ct;

	return ImColor();
}

ImColor cvisuals::get_health_color(int health)
{
	health = max(0, min(health, 100));

	return ImColor(min((510 * (100 - health)) / 100, 255), min((510 * health) / 100, 255), 0, 255);
}
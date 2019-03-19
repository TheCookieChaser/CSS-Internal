#include "visuals.h"
#include "../tools/math.h"

cvisuals* visuals = new cvisuals();

void render_text(int x, int y, Color color, HFont font, const wchar_t * text)
{
	g_surface->DrawSetTextPos(x, y);
	g_surface->DrawSetTextFont(font);
	g_surface->DrawSetTextColor(color);
	g_surface->DrawPrintText(text, wcslen(text));
}

const char* WeaponIDToAlias(int id)
{
	static auto function = reinterpret_cast<const char*(*)(int)>(
		tools::get_rel32(tools::find_pattern("client.dll", "E8 ? ? ? ? 50 FF 75 94"), 1, 5));
	return function(id);
}

void cvisuals::initialize()
{
	courier_new = g_surface->CreateFont();
	g_surface->SetFontGlyphSet(courier_new, "Courier New", 14, 0, 0, 0, ISurface::FONTFLAG_OUTLINE);

	initialized = true;
}

void cvisuals::render()
{
	render_text(10, 10, config.colors_watermark, courier_new, L"CSS-Internal");

	if (!config.visuals_enabled)
		return;

	render_esp();
	render_spread_circle();
}

void cvisuals::render_esp()
{
	auto local = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local)
		return;
	
	for (auto i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
	{
		auto entity = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(i));
		if (!entity || entity->IsDormant())
			continue;

		auto client_class = entity->GetClientClass();
		if (!client_class)
			continue;

		if (client_class->m_ClassID == CCSPlayer)
			render_player(entity);
	}
}

void cvisuals::render_player(C_CSPlayer* player)
{
	auto local = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
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
		if (config.visuals_box)
		{
			g_surface->DrawSetColor(get_player_color(player));
			g_surface->DrawOutlinedRect(rect.left, rect.top, rect.right, rect.bottom);
		}

		if (config.visuals_health)
		{
			g_surface->DrawSetColor(get_health_color(player->get_health()));
			g_surface->DrawOutlinedRect(rect.left, rect.bottom + 2,
				rect.left + (rect.right - rect.left) / 100.f * player->get_health(), rect.bottom + 2);
		}

		if (config.visuals_snapline)
		{
			g_surface->DrawSetColor(255, 255, 255, 255);
			g_surface->DrawLine(screen_width / 2.f, screen_height, rect.left + (rect.right - rect.left) / 2.f, rect.bottom);
		}

		wchar_t buf[128];
		if (config.visuals_name)
		{
			if (MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 128) > 0)
			{
				int w, h;
				g_surface->GetTextSize(courier_new, buf, w, h);
				render_text(rect.left + (rect.right - rect.left) / 2.f - (w / 2.f), rect.top - 16,
					Color(255, 255, 255, 255), courier_new, buf);
			}
		}

		if (config.visuals_weapon)
		{
			auto weapon_name = WeaponIDToAlias(weapon->GetWeaponID());
			if (!weapon_name)
				weapon_name = "unknown";

			if (MultiByteToWideChar(CP_UTF8, 0, weapon_name, -1, buf, 128) > 0)
			{
				int w, h;
				g_surface->GetTextSize(courier_new, buf, w, h);
				render_text(rect.left + (rect.right - rect.left) / 2.f - (w / 2.f), rect.bottom + 1,
					Color(255, 255, 255, 255), courier_new, buf);
			}
		}
	}
}

void cvisuals::render_spread_circle()
{
	if (!config.visuals_spread_circle)
		return;

	auto local_player = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local_player)
		return;

	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return;

	const auto accuracy = weapon->GetInaccuracy() * 550.f; //3000

	int screen_w, screen_h;
	g_surface->GetScreenSize(screen_w, screen_h);

	g_surface->DrawSetColor(255, 0, 0, 255);
	g_surface->DrawOutlinedCircle(screen_w / 2, screen_h / 2, accuracy, 360);
}

Color cvisuals::get_player_color(C_CSPlayer* pEntity)
{
	if (pEntity->get_team_num() == 2)
		return entity::is_visible(pEntity, 12) ? config.colors_esp_visible_t : config.colors_esp_t;
	if (pEntity->get_team_num() == 3)
		return entity::is_visible(pEntity, 12) ? config.colors_esp_visible_ct : config.colors_esp_ct;

	return Color();
}

Color cvisuals::get_health_color(int health)
{
	health = max(0, min(health, 100));

	return Color(min((510 * (100 - health)) / 100, 255), min((510 * health) / 100, 255), 0, 255);
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
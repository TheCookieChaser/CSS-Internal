#include "Visuals.h"

void Visuals::EndScene()
{
	ImColor Watermark_color(Settings::Colors::Watermark[0], Settings::Colors::Watermark[1],
		Settings::Colors::Watermark[2], 1.f);

	DrawManager->DrawText({ 10, 10 }, Watermark_color, (text_flags)NULL, "CSS-Internal");

	DrawESP();
}

D3DCOLOR Visuals::GetPlayerColor(C_CSPlayer* pEntity)
{
	ImColor T_color(Settings::Colors::ESP_NotVisible_T[0], Settings::Colors::ESP_NotVisible_T[1],
		Settings::Colors::ESP_NotVisible_T[2], 255.f);

	ImColor CT_color(Settings::Colors::ESP_NotVisible_CT[0], Settings::Colors::ESP_NotVisible_CT[1],
		Settings::Colors::ESP_NotVisible_CT[2], 255.f);

	ImColor T_visible_color(Settings::Colors::ESP_Visible_T[0], Settings::Colors::ESP_Visible_T[1],
		Settings::Colors::ESP_Visible_T[2], 255.f);

	ImColor CT_visible_color(Settings::Colors::ESP_Visible_CT[0], Settings::Colors::ESP_Visible_CT[1],
		Settings::Colors::ESP_Visible_CT[2], 255.f);

	if (pEntity->GetTeamNum() == 2)
		return Entity::IsVisible(pEntity, 12) ? T_visible_color : T_color;
	else if (pEntity->GetTeamNum() == 3)
		return Entity::IsVisible(pEntity, 12) ? CT_visible_color : CT_color;

	return ImColor();
}

D3DCOLOR Visuals::GetHealthColor(int health)
{
	health = max(0, min(health, 100));

	return ImColor(min((510 * (100 - health)) / 100, 255), min((510 * health) / 100, 255), 0, 255);
}

void Visuals::DrawESP()
{
	if (!Settings::Visuals::Enalbed)
		return;

	auto pLocal = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));

	if (!pLocal)
		return;

	for (int i = 0; i < entitylist->GetHighestEntityIndex(); i++)
	{
		auto pEntity = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(i));

		if (!pEntity
			|| pEntity->IsDormant()
			|| pEntity->GetHealth() <= 0
			|| pEntity->GetLifeState() == 1
			|| pEntity->GetTeamNum() == pLocal->GetTeamNum()
			|| pEntity == pLocal)
			continue;

		Vector mins, maxs;
		Vector pos3D, top3D;
		D3DXVECTOR2 pos, top;
		pEntity->GetRenderBounds(mins, maxs);
		pos3D = pEntity->GetVecOrigin();
		top3D = pos3D + Vector(0, 0, maxs.z);
		if (!DrawManager->WorldToScreen(pos3D, pos) || !DrawManager->WorldToScreen(top3D, top))
			continue;

		float height = (pos.y - top.y);
		float width = height / 4.f;

		player_info_t info;
		if (!engine->GetPlayerInfo(i, &info))
			continue;

		auto pWeapon = pEntity->GetActiveWeapon();

		if (!pWeapon)
			continue;

		if (Settings::Visuals::Box)
		{
			DrawManager->DrawRect({ top.x - width, top.y }, { top.x + width, top.y + height }, GetPlayerColor(pEntity));
			DrawManager->DrawRect({ top.x - width - 1, top.y - 1 }, { top.x + width + 1, top.y + height + 1 }, ImColor(0, 0, 0, 255));
			DrawManager->DrawRect({ top.x - width + 1, top.y + 1 }, { top.x + width - 1, top.y + height - 1 }, ImColor(0, 0, 0, 255));
		}

		if (Settings::Visuals::Health)
		{
			UINT pos = height - (UINT)((pEntity->GetHealth() * height) / 100);
			DrawManager->DrawRectFilled({ top.x - width - 6, top.y + pos }, { top.x - width - 2, top.y + height }, GetHealthColor(pEntity->GetHealth()));
			DrawManager->DrawRect({ top.x - width - 6 , top.y }, { top.x - width - 2, top.y + height }, ImColor(0, 0, 0, 255));
		}

		if (Settings::Visuals::SnapLine)
		{
			int screen_width, screen_height;
			engine->GetScreenSize(screen_width, screen_height);

			DrawManager->DrawLine({ screen_width / 2.f, (float)screen_height }, { top.x, top.y + height }, ImColor(255, 255, 255, 255));
		}

		if (Settings::Visuals::Name)
		{
			DrawManager->DrawText({ top.x, top.y - 16 }, ImColor(255, 255, 255, 255), text_flags::centered_x, info.szName);
		}

		if (Settings::Visuals::Weapon)
		{
			DrawManager->DrawText({ top.x, top.y + height }, ImColor(255, 255, 255, 255), text_flags::centered_x, pWeapon->GetName());
		}
	}
}
#include "Aimbot.h"

C_CSPlayer* Aimbot::GetBestTarget(CUserCmd* pCmd)
{
	C_CSPlayer* bestentity = nullptr;
	auto bestfov = Settings::Aimbot::Fov;

	auto pLocal = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	for (int i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
	{
		auto pEntity = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(i));

		if (!pEntity
			|| pEntity->IsDormant()
			|| pEntity->get_health() <= 0
			|| pEntity->get_life_state() == 1
			|| pEntity->get_team_num() == pLocal->get_team_num()
			|| pEntity == pLocal
			|| !Entity::IsVisible(pEntity, 12))
			continue;

		auto fov = Math::GetFov(pCmd->viewangles, Math::CalcAngle(pLocal->get_eye_position(), Entity::GetHitboxPosition(pEntity, Settings::Aimbot::Hitbox)));

		if (fov < bestfov)
		{
			bestfov = fov;
			bestentity = pEntity;
		}
	}

	return bestentity;
}

void Aimbot::CreateMove(CUserCmd* pCmd)
{
	if (!Settings::Aimbot::Enalbed)
		return;

	auto pLocal = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	auto pEntity = GetBestTarget(pCmd);

	if (!pEntity)
		return;

	auto ang = Math::CalcAngle(pLocal->get_eye_position(), Entity::GetHitboxPosition(pEntity, Settings::Aimbot::Hitbox));
	ang -= pLocal->get_aim_punch() * 2.f;
	Math::NormalizeAngles(ang);

	if (Settings::Aimbot::Smooth)
	{
		auto delta = ang - pCmd->viewangles;
		Math::NormalizeAngles(delta);
		delta /= Settings::Aimbot::Smooth;
		ang = delta + pCmd->viewangles;
	}

	Math::NormalizeAngles(ang);
	Math::ClampAngles(ang);

	Vector oldangles = pCmd->viewangles;
	float oldside = pCmd->sidemove;
	float oldforward = pCmd->forwardmove;

	if (!Settings::Aimbot::OnKey || (Settings::Aimbot::OnKey && PressedKeys[Settings::Aimbot::Key]))
	{
		pCmd->viewangles = ang;
		if (!Settings::Aimbot::Silent)
			g_engine->SetViewAngles(ang);

		if (Settings::Aimbot::AutoFire)
			pCmd->buttons |= IN_ATTACK;
	}

	Math::CorrectMovement(pCmd->viewangles, pCmd, oldforward, oldside);
}
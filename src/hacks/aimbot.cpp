#include "aimbot.h"
#include "../tools/draw_manager.h"

caimbot* aimbot = new caimbot();

void caimbot::move(CUserCmd* pCmd)
{
	if (!config.aimbot_enabled)
		return;

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	auto entity = get_best_target();

	if (!entity)
		return;

	Vector current_angles;
	g_engine->GetViewAngles(current_angles);

	auto hitboxpos = entity::get_hitbox_position(entity, config.aimbot_hitbox);
	auto delta = local->get_eye_position() - hitboxpos;
	Vector angles;
	math::VectorAngles(delta, angles);
	angles -= local->get_aim_punch() * 2.f;
	math::clamp_angles(angles);

	if (config.aimbot_smooth)
	{
		delta = angles - current_angles;
		math::clamp_angles(delta);
		delta /= config.aimbot_smooth;
		math::clamp_angles(delta);
		angles = delta + current_angles;
	}

	math::clamp_angles(angles);

	Vector oldangles = pCmd->viewangles;
	float oldside = pCmd->sidemove;
	float oldforward = pCmd->forwardmove;

	if (!config.aimbot_on_key || (config.aimbot_on_key && PressedKeys[config.aimbot_key]))
	{
		pCmd->viewangles = angles;
		if (!config.aimbot_silent)
			g_engine->SetViewAngles(angles);

		if (config.aimbot_autofire)
			pCmd->buttons |= IN_ATTACK;
	}

	math::correct_movement(pCmd->viewangles, pCmd, oldforward, oldside);
}

C_CSPlayer* caimbot::get_best_target()
{
	C_CSPlayer* bestentity = nullptr;
	auto bestfov = config.aimbot_fov;

	Vector current_angles;
	g_engine->GetViewAngles(current_angles);

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	for (int i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
	{
		auto entity = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(i));

		if (!entity
			|| entity->IsDormant()
			|| entity->get_health() <= 0
			|| entity->get_life_state() == 1
			|| entity->get_team_num() == local->get_team_num()
			|| entity == local
			|| !entity::is_visible(entity, 12))
			continue;

		auto delta = local->get_eye_position() - entity::get_hitbox_position(entity, config.aimbot_hitbox);
		Vector angles;
		math::VectorAngles(delta, angles);
		math::clamp_angles(angles);

		delta = angles - current_angles;
		delta.y = std::remainderf(delta.y, 360.f);
		auto fov = delta.Length2D();

		if (fov < bestfov)
		{
			bestfov = fov;
			bestentity = entity;
		}
	}

	return bestentity;
}
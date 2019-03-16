#include "aimbot.h"
#include "../tools/draw_manager.h"
#include "../tools/game.h"

caimbot* aimbot = new caimbot();

void apply_nospread_angles(CUserCmd* cmd, C_BasePlayer* local_player, Vector& viewangles)
{
	//if (!(cmd->buttons & IN_ATTACK))
	//	return;

	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return;

	//const auto server_time = local_player->get_tick_base() * g_globalvars->interval_per_tick;
	//if (server_time <= weapon->get_next_primary_attack() || server_time <= local_player->get_next_attack())
	//	return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	weapon->UpdateAccuracyPenalty();

	game::RandomSeed((cmd->random_seed & 0xFF) + 1);

	const auto rand1 = game::RandomFloat(0.f, M_PI_F * 2.f);
	const auto rand2 = game::RandomFloat(0.f, weapon->GetInaccuracy());

	const auto v37 = cosf(rand1) * rand2;
	const auto v38 = sinf(rand1) * rand2;

	auto vec_start = local_player->get_eye_position();

	const auto rand3 = game::RandomFloat(0.0, M_PI_F * 2.f);
	const auto rand4 = game::RandomFloat(0.0, weapon->GetSpread());

	const auto spread_x = cosf(rand3) * rand4 + v37;
	const auto spread_y = sinf(rand3) * rand4 + v38;

	Vector forward, right, up;
	math::AngleVectors(viewangles, &forward, &right, &up);

	auto vec_dir = forward + (right * spread_x * -1.f) + (up * spread_y * -1.f);
	VectorNormalize(vec_dir);

	//i have no fucking idea why but i have to negate this
	math::VectorAngles(-vec_dir, viewangles);

	math::clamp_angles(viewangles);
}

void GetBulletTypeParameters(int iBulletType, float &fPenetrationPower, float &flPenetrationDistance)
{
	static auto function = reinterpret_cast<void(__stdcall*)(int, float&, float&)>(
		tools::get_rel32(tools::find_pattern("client.dll", "E8 ? ? ? ? F3 0F 10 43 ? 33 FF"), 1, 5));
	function(iBulletType, fPenetrationPower, flPenetrationDistance);
}

void GetMaterialParameters(int iMaterial, float &flPenetrationModifier, float &flDamageModifier)
{
	static auto function = reinterpret_cast<void(*)(int, float&, float&)>(
		tools::find_pattern("client.dll", "55 8B EC 8B 45 08 83 C0 BD"));
	function(iMaterial, flPenetrationModifier, flDamageModifier);
}

bool TraceToExit(Vector &start, Vector &dir, Vector &end, float flStepSize, float flMaxDistance)
{
	//float flDistance = 0;
	//while (flDistance <= flMaxDistance)
	//{
	//	flDistance += flStepSize;

	//	end = start + dir * flDistance;

	//	auto point_contents = g_trace->GetPointContents(end, 0);

	//	if (!(point_contents & MASK_SOLID))
	//	{
	//		// found first free point
	//		return true;
	//	}
	//}

	return false;
}

bool can_penetrate_point(C_BasePlayer* local_player, C_BasePlayer* target, Vector origin, Vector destination, float& damage)
{
	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return 0.f;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return 0.f;

	float penetration_power, penetration_distance;
	GetBulletTypeParameters(weapon_data->iAmmoType, penetration_power, penetration_distance);

	static Vector current_origin, direction, penetration_end;
	current_origin = origin;
	direction = destination - origin;
	const auto length = direction.NormalizeInPlace();

	VectorNormalize(direction);

	auto final_length = 0.f;
	auto current_penetration = weapon_data->m_iPenetration;
	auto current_damage = static_cast<float>(weapon_data->m_iDamage);

	CTraceFilter enter_filter, exit_filter;
	trace_t enter_trace, exit_trace;

	enter_filter.pSkip = local_player;

	while (current_damage > 0.f)
	{
		game::UTIL_TraceLine(current_origin, destination,
			MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, &enter_filter, &enter_trace);
		game::UTIL_ClipTraceToPlayers(current_origin, destination + direction * 40.f,
			MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, &enter_filter, &enter_trace);

		auto player = reinterpret_cast<C_CSPlayer*>(enter_trace.m_pEnt);
		if (enter_trace.fraction == 1.f)
			break;

		const auto grate = enter_trace.contents & CONTENTS_GRATE;
		const auto current_length = final_length + (length - final_length) * enter_trace.fraction;
		const auto raised_damage_modifier = std::pow(
			weapon_data->m_flRangeModifier,
			enter_trace.fraction * (current_length * 0.002f)
		);

		current_damage *= raised_damage_modifier;
		if (enter_trace.hitgroup != HITGROUP_GEAR && enter_trace.hitgroup != 0 && player && target && enter_trace.m_pEnt == target)
		{
			//const auto scaled_damage = scale_player_damage(
			//	target,
			//	current_damage,
			//	weapon_info.m_ArmorRatio,
			//	enter_trace.m_Hitgroup,
			//	mp_friendlyfire,
			//	player->get_team() == local_team
			//);
			//return scaled_damage >= static_cast<float>(min_damage);

			damage = current_damage;
			return true;
		}

		auto* enter_surface = g_physprops->GetSurfaceData(enter_trace.surface.surfaceProps);
		if (!enter_surface)
			break;

		// out of range
		if (current_length > penetration_distance)
			break;

		// check if bullet can penetarte another entity
		if (current_penetration == 0 && !grate)
			break;

		// If we hit a grate with current_penetration == 0, stop on the next thing we hit
		if (current_penetration < 0)
			break;

		const auto enter_material = enter_surface->game.material;
		auto penetration_power_modifier = 0.f, damage_modifier = 0.f;
		GetMaterialParameters(enter_material, penetration_power_modifier, damage_modifier);

		// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
		if (grate)
		{
			penetration_power_modifier = 1.0f;
			damage_modifier = 0.99f;
		}

		// try to penetrate object, maximum penetration is 128 inch
		if (!TraceToExit(enter_trace.endpos, direction, penetration_end, 24.f, 128.f))
			break;

		game::UTIL_TraceLine(
			penetration_end,
			enter_trace.endpos,
			MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX,
			nullptr,
			&exit_trace
		);

		if (exit_trace.m_pEnt && exit_trace.m_pEnt != enter_trace.m_pEnt)
		{
			exit_filter.pSkip = exit_trace.m_pEnt;
			game::UTIL_TraceLine(
				penetration_end,
				enter_trace.endpos,
				MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX,
				&exit_filter,
				&exit_trace
			);
		}

		const auto penetration_length = (exit_trace.endpos - enter_trace.endpos).NormalizeInPlace();
		const auto* exit_material = g_physprops->GetSurfaceData(exit_trace.surface.surfaceProps);
		if (exit_material                                      &&
			exit_material->game.material == enter_material &&
			(enter_material == 87 /*CHAR_TEX_WOOD*/ ||
				enter_material == 77 /*CHAR_TEX_METAL*/)) {
			penetration_power_modifier += penetration_power_modifier;
		}
		if (penetration_length > penetration_power * penetration_power_modifier) {
			break;
		}

		// set the last exit trace position as the next enter trace position
		current_origin = exit_trace.endpos;
		final_length = current_length + penetration_length;

		penetration_power -= penetration_length / penetration_power_modifier;
		// calculate the new possible damage for the next penetration
		current_damage *= damage_modifier;

		--current_penetration;
	}

	return false;
}

void caimbot::move(CUserCmd* pCmd)
{
	if (!config.aimbot_enabled)
		return;

	auto local_player = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local_player)
		return;

	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return;

	const auto server_time = local_player->get_tick_base() * g_globalvars->interval_per_tick;
	if (server_time <= weapon->get_next_primary_attack() || server_time <= local_player->get_next_attack())
		return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	auto entity = get_best_target();
	if (!entity)
		return;

	Vector current_angles;
	g_engine->GetViewAngles(current_angles);

	auto hitboxpos = entity::get_hitbox_position(entity, config.aimbot_hitbox);
	auto delta = local_player->get_eye_position() - hitboxpos;
	Vector angles;
	math::VectorAngles(delta, angles);
	angles -= local_player->get_aim_punch() * 2.f;
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
		if (config.aimbot_nospread)
			apply_nospread_angles(pCmd, local_player, angles);
		pCmd->viewangles = angles;
		if (!config.aimbot_silent)
			g_engine->SetViewAngles(angles);

		if (config.aimbot_autofire)
			pCmd->buttons |= IN_ATTACK;
	}

	math::correct_movement(oldangles, pCmd, oldforward, oldside);
}

C_CSPlayer* caimbot::get_best_target()
{
	C_CSPlayer* bestentity = nullptr;
	auto bestfov = config.aimbot_fov;

	Vector current_angles;
	g_engine->GetViewAngles(current_angles);

	auto local = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	for (auto i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
	{
		auto entity = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(i));

		if (!entity
			|| entity->IsDormant()
			|| entity->get_health() <= 0
			|| entity->get_life_state() == 1
			|| entity->get_team_num() == local->get_team_num()
			|| entity == local)
			continue;

		float damage = 0.f;
		if (!can_penetrate_point(local, entity, local->get_eye_position(), entity::get_hitbox_position(entity, config.aimbot_hitbox), damage))
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
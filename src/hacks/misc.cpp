#include "misc.h"
#include "../tools/draw_manager.h"
#include "../tools/game.h"
#include "../tools/math.h"

cmisc* misc = new cmisc();

void triggerbot(CUserCmd* cmd, C_BasePlayer* local_player, int custom_seed = -1)
{
	if (config.misc_triggerbot_on_key && !GetAsyncKeyState(config.misc_triggerbot_key))
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

	weapon->UpdateAccuracyPenalty();

	if (custom_seed == -1)
		game::RandomSeed((cmd->random_seed & 0xFF) + 1);
	else
		game::RandomSeed(custom_seed + 1);

	const auto rand1 = game::RandomFloat(0.f, M_PI_F * 2.f);
	const auto rand2 = game::RandomFloat(0.f, weapon->GetInaccuracy());

	const auto v37 = cosf(rand1) * rand2;
	const auto v38 = sinf(rand1) * rand2;

	std::array<Vector2D, 16> spreads;

	auto vec_start = local_player->get_eye_position();
	auto viewangles = cmd->viewangles;
	viewangles -= local_player->get_aim_punch() * 2.f;

	for (auto i = 0; i < weapon_data->m_iBullets; i++)
	{
		const auto rand3 = game::RandomFloat(0.0, M_PI_F * 2.f);
		const auto rand4 = game::RandomFloat(0.0, weapon->GetSpread());

		spreads[i].x = cosf(rand3) * rand4;
		spreads[i].y = sinf(rand3) * rand4;

		const auto final_spread_x = spreads[i].x + v37;
		const auto final_spread_y = spreads[i].y + v38;

		Vector forward, right, up;
		math::AngleVectors(viewangles, &forward, &right, &up);

		auto vec_dir = forward + (right * final_spread_x) + (up * final_spread_y);
		VectorNormalize(vec_dir);

		float flMaxRange = 8000;

		auto vec_end = vec_start + vec_dir * flMaxRange; // max bullet range is 10000 units

		Ray_t ray;
		ray.Init(vec_start, vec_end);

		CTraceFilter filter;
		filter.pSkip = local_player;

		trace_t tr;

		g_trace->TraceRay(ray, 0x4600400B, &filter, &tr);

		if (config.visuals_triggerbot_seed)
		{
			Vector screen_position;
			auto color = ImColor(0, 255, 0, 255);
			if (custom_seed == (cmd->random_seed & 0xFF) || custom_seed == -1)
				color = ImColor(255, 0, 0, 255);

			if (!g_debugoverlay->ScreenPosition(tr.endpos, screen_position))
				drawmanager->add_filled_rect({ screen_position.x - 2, screen_position.y - 2 },
					{ screen_position.x + 2, screen_position.y + 2 }, color);
		}

		// Check for player hitboxes extending outside their collision bounds
		const float rayExtension = 40.0f;
		game::UTIL_ClipTraceToPlayers(vec_start, vec_end + vec_dir * rayExtension, 0x4600400B, &filter, &tr);

		if (tr.fraction == 1.0f)
			continue; // we didn't hit anything, stop tracing shoot

		auto entity = reinterpret_cast<C_BasePlayer*>(tr.m_pEnt);
		if (!entity || entity->get_life_state() == 1)
			continue;

		if (!config.misc_triggerbot_filter[5] && entity->get_team_num() == local_player->get_team_num())
			continue;

		auto can_continue = false;
		if (config.misc_triggerbot_filter[0] && tr.hitgroup == HITGROUP_HEAD)
			can_continue = true;
		if (config.misc_triggerbot_filter[1] && tr.hitgroup == HITGROUP_CHEST)
			can_continue = true;
		if (config.misc_triggerbot_filter[2] && tr.hitgroup == HITGROUP_STOMACH)
			can_continue = true;
		if (config.misc_triggerbot_filter[3] && (tr.hitgroup == HITGROUP_LEFTARM || tr.hitgroup == HITGROUP_RIGHTARM))
			can_continue = true;
		if (config.misc_triggerbot_filter[4] && (tr.hitgroup == HITGROUP_LEFTLEG || tr.hitgroup == HITGROUP_RIGHTLEG))
			can_continue = true;

		if (!can_continue)
			continue;

		if (custom_seed != -1)
		{
			cmd->random_seed = custom_seed & 0x7FFFFFFF;

			//Generate a command number for the custom seed
			unsigned cmd_number = 0;
			while ((MD5_PseudoRandom(cmd_number) & 0xFF) != custom_seed)
				cmd_number++;

			cmd->command_number = cmd_number;
		}

		cmd->buttons |= IN_ATTACK;

		if (config.visuals_triggerbot_hit_point)
			g_debugoverlay->AddBoxOverlay(tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 255, 0, 0, 127, 4);

		if (config.visuals_triggerbot_hitbox)
		{
			static auto DrawClientHitboxes = reinterpret_cast<void(__thiscall*)(void*, float, bool)>(tools::find_pattern("client.dll", "55 8B EC 83 EC 60 57"));
			DrawClientHitboxes(tr.m_pEnt, 4.f, false);
		}
	}
}

void cmisc::move(CUserCmd* cmd)
{
	auto local_player = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local_player)
		return;

	switch (config.misc_triggerbot)
	{
	case 1:
		triggerbot(cmd, local_player);
		break;
	case 2:
		{
			for (auto i = 0; i < 256; i++)
				triggerbot(cmd, local_player, i);
			break;
		}
	default:
		break;
	}
}

void cmisc::autojump(CUserCmd* cmd)
{
	auto local_player = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local_player)
		return;

	if (config.misc_autojump)
	{
		if (GetAsyncKeyState(VK_SPACE) && !(local_player->get_flags() & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;
	}
}
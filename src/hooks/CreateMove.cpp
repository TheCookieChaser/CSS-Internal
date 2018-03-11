#include "hooks.h"
#include "../tools/entity.h"
#include "../tools/math.h"
#include "../tools/config.h"
#include "../tools/game.h"
#include "../hacks/aimbot.h"
#include "../hacks/visuals.h"
#include "../hacks/misc.h"

void bruteforce_triggerbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon)
{
	const auto server_time = local->get_tick_base() * g_globalvars->interval_per_tick;
	if (server_time <= weapon->GetNextPrimaryAttack())
		return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	weapon->UpdateAccuracyPenalty();

	const auto inaccuracy = weapon->GetInaccuracy();
	const auto spread = weapon->GetSpread();

	auto vec_start = local->get_eye_position();

	auto viewangles = cmd->viewangles;
	viewangles += local->get_aim_punch() * 2.f;

	for (int i = 0; i < 256; i++)
	{
		game::RandomSeed(i + 1);

		const auto rand1 = game::RandomFloat(0.0, M_PI_F * 2.f);
		const auto rand2 = game::RandomFloat(0.0, inaccuracy);
		const auto v37 = rand2 * cosf(rand1);
		const auto v38 = rand2 * sinf(rand1);

		for (auto bullet = 0; bullet < weapon_data->Bullets; ++bullet)
		{
			const auto rand3 = game::RandomFloat(0.0, M_PI_F * 2.f);
			const auto rand4 = game::RandomFloat(0.0, spread);

			const auto spreads_x = rand4 * cosf(rand3) + v37;
			const auto spreads_y = rand4 * sinf(rand3) + v38;

			Vector vec_dir_shooting, vec_right, vec_up;
			math::AngleVectors(viewangles, &vec_dir_shooting, &vec_right, &vec_up);

			auto vec_dir = vec_dir_shooting + (vec_right * spreads_x) + (vec_up * spreads_y);
			vec_dir.NormalizeInPlace();

			auto vec_end = vec_start + vec_dir * weapon_data->Range;

			Ray_t ray;
			ray.Init(vec_start, vec_end);

			CTraceFilter filter;
			filter.pSkip = local;

			trace_t tr;

			g_trace->TraceRay(ray, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);

			Vector end_screen;
			if (!g_debugoverlay->ScreenPosition(tr.endpos, end_screen))
			{
				if ((cmd->random_seed & 0xFF) == i)
				{
					drawmanager->add_filled_circle({ end_screen.x, end_screen.y }, 2, ImColor(255, 0, 0, 255));
				}
				else
				{
					drawmanager->add_filled_circle({ end_screen.x, end_screen.y }, 2, ImColor(0, 0, 255, 255));
				}
			}

			if (tr.m_pEnt && reinterpret_cast<C_CSPlayer*>(tr.m_pEnt)->get_team_num() != local->get_team_num() && tr.hitgroup > 0 && tr.hitgroup <= 1)
			{
				cmd->buttons |= IN_ATTACK;
				cmd->random_seed = i & 0x7FFFFFFF;

				unsigned int cmdnumber = 0;

				while ((MD5_PseudoRandom(cmdnumber) & 0xFF) != i) cmdnumber++;

				cmd->command_number = cmdnumber;

				//static auto DrawClientHitboxes = reinterpret_cast<void(__thiscall*)(void*, float, bool)>(tools::find_pattern("client.dll", "55 8B EC 83 EC 60 57"));

				//DrawClientHitboxes(tr.m_pEnt, 4.f, false);
				//g_debugoverlay->AddBoxOverlay(tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 255, 0, 127, 4.f);
			}
		}
	}
}

void triggerbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon)
{
	const auto server_time = local->get_tick_base() * g_globalvars->interval_per_tick;
	if (server_time <= weapon->GetNextPrimaryAttack())
		return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	weapon->UpdateAccuracyPenalty();

	game::RandomSeed((cmd->random_seed & 0xFF) + 1);

	const auto rand1 = game::RandomFloat(0.0, M_PI_F * 2.f);
	const auto rand2 = game::RandomFloat(0.0, weapon->GetInaccuracy());
	const auto v37 = rand2 * cosf(rand1);
	const auto v38 = rand2 * sinf(rand1);

	auto vec_start = local->get_eye_position();

	auto viewangles = cmd->viewangles;
	viewangles += local->get_aim_punch() * 2.f;

	for (auto bullet = 0; bullet < weapon_data->Bullets; bullet++)
	{
		const auto rand3 = game::RandomFloat(0.0, M_PI_F * 2.f);
		const auto rand4 = game::RandomFloat(0.0, weapon->GetSpread());

		const auto spreads_x = rand4 * cosf(rand3) + v37;
		const auto spreads_y = rand4 * sinf(rand3) + v38;

		Vector vec_dir_shooting, vec_right, vec_up;
		math::AngleVectors(viewangles, &vec_dir_shooting, &vec_right, &vec_up);

		auto vec_dir = vec_dir_shooting + (vec_right * spreads_x) + (vec_up * spreads_y);
		vec_dir.NormalizeInPlace();

		auto vec_end = vec_start + vec_dir * weapon_data->Range;

		Ray_t ray;
		ray.Init(vec_start, vec_end);

		CTraceFilter filter;
		filter.pSkip = local;

		trace_t tr;

		g_trace->TraceRay(ray, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);

		Vector screen;
		if (!g_debugoverlay->ScreenPosition(tr.endpos, screen))
			drawmanager->add_filled_rect({ screen.x - 2, screen.y - 2 }, { screen.x + 2, screen.y + 2 }, ImColor(255, 0, 0, 255));

		if (!tr.m_pEnt || tr.hitgroup == 0)
			continue;

		cmd->buttons |= IN_ATTACK;

		static auto draw_client_hitboxes = reinterpret_cast<void(__thiscall*)(void*, float, bool)>(tools::find_pattern("client.dll", "55 8B EC 83 EC 60 57"));

		draw_client_hitboxes(tr.m_pEnt, 4.f, false);
		g_debugoverlay->AddBoxOverlay(tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 255, 0, 127, 4.f);
	}
}

void engine_prediction(C_BasePlayer* local_player, CUserCmd* cmd)
{
	static CMoveData g_pMoveData;
	ZeroMemory(&g_pMoveData, sizeof(CMoveData));

	//static auto g_pMoveData = *reinterpret_cast<CMoveData**>(
	//	tools::find_pattern("client.dll", "FF 35 ? ? ? ? 57") + 2);
	static auto m_nPredictionRandomSeed = *reinterpret_cast<int**>(
		tools::find_pattern("client.dll", "A3 ? ? ? ? 5D C3 55 8B EC 8B 45 08") + 1);
	static auto m_pPredictionPlayer = *reinterpret_cast<int**>(
		tools::find_pattern("client.dll", "89 3D ? ? ? ? F3 0F 2A 87") + 2);

	*reinterpret_cast<CUserCmd**>(local_player + 0x101C) = cmd;

	*m_nPredictionRandomSeed = cmd->random_seed;
	*m_pPredictionPlayer = reinterpret_cast<std::uintptr_t>(local_player);

	g_globalvars->curtime = local_player->get_tick_base() * g_globalvars->interval_per_tick;
	g_globalvars->frametime = g_globalvars->interval_per_tick;

	g_gamemovement->StartTrackPredictionErrors(local_player);

	const auto v2 = cmd->buttons ^ *reinterpret_cast<std::uintptr_t *>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1018);
	*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x100C)
	= *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1018);
	*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1018) = cmd->buttons;
	*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1010) = cmd->buttons & v2;
	*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1014) = v2 & ~cmd->buttons;

	g_prediction->SetupMove(local_player, cmd, g_movehelper, &g_pMoveData);

	g_gamemovement->ProcessMovement(local_player, &g_pMoveData);

	g_prediction->FinishMove(local_player, cmd, &g_pMoveData);

	g_gamemovement->FinishTrackPredictionErrors(local_player);

	*reinterpret_cast<CUserCmd**>(local_player + 0x101C) = nullptr;

	*m_nPredictionRandomSeed = -1;
	*m_pPredictionPlayer = 0;
}

void __stdcall _CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	o_create_move(sequence_number, input_sample_frametime, active);

	auto cmd = &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	auto verified = &g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

	if (!cmd || !verified)
		return;

	drawmanager->clear();

	auto local_player = static_cast<C_BasePlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (local_player)
	{
		misc->move(cmd);

		//engine_prediction(local_player, cmd);

		aimbot->move(cmd);
		visuals->render();

		if (GetAsyncKeyState(5))
		{
			auto weapon = local_player->GetActiveWeapon();
			if (weapon)
			{
				bruteforce_triggerbot(cmd, local_player, weapon);
			}
		}
	}

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();
}

void __declspec(naked) __stdcall create_move(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		LEA  EAX, [EBP - 01]
		PUSH EAX
		PUSH[ESP + 0x10]
		PUSH[ESP + 0x10]
		PUSH[ESP + 0x10]
		CALL _CreateMove
		RET 0x0C
	}
}
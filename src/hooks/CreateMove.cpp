#include "hooks.h"
#include "../tools/entity.h"
#include "../tools/math.h"
#include "../tools/config.h"
#include "../tools/game.h"
#include "../hacks/aimbot.h"
#include "../hacks/visuals.h"
#include "../hacks/misc.h"
#include <vector>

typedef enum
{
	Primary_Mode = 0,
	Secondary_Mode,
} SDKWeaponMode;

bool can_hit_point(C_CSPlayer* local, C_BaseCombatWeapon* weapon)
{
	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return false;

	auto iMode = Primary_Mode; //todo
	auto  iWeaponID = weapon->GetWeaponID();

	bool	bPrimaryMode = (iMode == Primary_Mode);
	int		iDamage = weapon_data->Damage;
	float	flRange = weapon_data->Range;
	int		iPenetration = weapon_data->Penetration;
	float	flRangeModifier = weapon_data->RangeModifier;
	int		iAmmoType = weapon_data->iAmmoType;

	float fCurrentDamage = weapon_data->Damage;   // damage of the bullet at it's current trajectory
	float flCurrentDistance = 0.0;  //distance that the bullet has traveled so far

	float flPenetrationPower = 0;		// thickness of a wall that this bullet can penetrate
	float flPenetrationDistance = 0;	// distance at which the bullet is capable of penetrating a wall
	float flDamageModifier = 0.5;		// default modification of bullets power after they go through a wall.
	float flPenetrationModifier = 1.f;

	if (!bPrimaryMode)
	{
		if (weapon->GetWeaponID() == WEAPON_GLOCK)
		{
			iDamage = 18;	// reduced power for burst shots
			flRangeModifier = 0.9f;
		}
		else if (iWeaponID == WEAPON_M4A1)
		{
			flRangeModifier = 0.95f; // slower bullets in silenced mode
		}
		else if (iWeaponID == WEAPON_USP)
		{
			iDamage = 30; // reduced damage in silenced mode
		}
	}

	while (fCurrentDamage > 0)
	{
		/*Vector vecEnd = vecSrc + vecDir * flDistance;

		trace_t tr; // main enter bullet trace

		UTIL_TraceLineIgnoreTwoEntities(vecSrc, vecEnd, CS_MASK_SHOOT | CONTENTS_HITBOX, this, lastPlayerHit, COLLISION_GROUP_NONE, &tr);
		{
			CTraceFilterSkipTwoEntities filter(this, lastPlayerHit, COLLISION_GROUP_NONE);

			// Check for player hitboxes extending outside their collision bounds
			const float rayExtension = 40.0f;
			UTIL_ClipTraceToPlayers(vecSrc, vecEnd + vecDir * rayExtension, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);
		}*/
	}
}

void bruteforce_triggerbot(CUserCmd* cmd, C_CSPlayer* local, C_BaseCombatWeapon* weapon)
{
	auto server_time = local->get_tick_base() * g_globalvars->interval_per_tick;
	if (server_time <= weapon->GetNextPrimaryAttack())
		return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	weapon->UpdateAccuracyPenalty();

	auto inaccuracy = weapon->GetInaccuracy();
	auto spread = weapon->GetSpread();

	auto vecStart = local->get_eye_position();

	auto viewangles = cmd->viewangles;
	viewangles += local->get_aim_punch() * 2.f;

	for (int i = 0; i < 256; i++)
	{
		game::RandomSeed(i + 1);

		auto rand1 = game::RandomFloat(0.0, M_PI * 2.f);
		auto rand2 = game::RandomFloat(0.0, inaccuracy);
		auto v37 = rand2 * cosf(rand1);
		auto v38 = rand2 * sinf(rand1);

		std::array<float, 16>spreads_x;
		std::array<float, 16>spreads_y;

		for (auto iBullet = 0; iBullet < weapon_data->Bullets; ++iBullet)
		{
			auto rand3 = game::RandomFloat(0.0, M_PI * 2.f);
			auto rand4 = game::RandomFloat(0.0, spread);

			spreads_x.at(iBullet) = rand4 * cosf(rand3);
			spreads_y.at(iBullet) = rand4 * sinf(rand3);

			spreads_x.at(iBullet) += v37;
			spreads_y.at(iBullet) += v38;

			Vector vecDirShooting, vecRight, vecUp;
			math::AngleVectors(viewangles, &vecDirShooting, &vecRight, &vecUp);

			auto vecDir = vecDirShooting + (vecRight * spreads_x.at(iBullet)) + (vecUp * spreads_y.at(iBullet));
			vecDir.NormalizeInPlace();

			auto vecEnd = vecStart + vecDir * weapon_data->Range;

			Ray_t ray;
			ray.Init(vecStart, vecEnd);

			CTraceFilter filter;
			filter.pSkip = local;

			trace_t tr;

			g_trace->TraceRay(ray, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);

			Vector start_screen, end_screen;
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

void triggerbot(CUserCmd* cmd, C_CSPlayer* local, C_BaseCombatWeapon* weapon)
{
	auto server_time = local->get_tick_base() * g_globalvars->interval_per_tick;
	if (server_time <= weapon->GetNextPrimaryAttack())
		return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	weapon->UpdateAccuracyPenalty();

	game::RandomSeed((cmd->random_seed & 0xFF) + 1);

	auto rand1 = game::RandomFloat(0.0, M_PI * 2.f);
	auto rand2 = game::RandomFloat(0.0, weapon->GetInaccuracy());
	auto v37 = rand2 * cosf(rand1);
	auto v38 = rand2 * sinf(rand1);

	Vector vecStart = local->get_eye_position();

	auto viewangles = cmd->viewangles;
	viewangles += local->get_aim_punch() * 2.f;

	std::array<float, 16>spreads_x;
	std::array<float, 16>spreads_y;

	for (auto iBullet = 0; iBullet < weapon_data->Bullets; ++iBullet)
	{
		auto rand3 = game::RandomFloat(0.0, M_PI * 2.f);
		auto rand4 = game::RandomFloat(0.0, weapon->GetSpread());

		spreads_x.at(iBullet) = rand4 * cosf(rand3);
		spreads_y.at(iBullet) = rand4 * sinf(rand3);

		spreads_x.at(iBullet) += v37;
		spreads_y.at(iBullet) += v38;

		Vector vecDirShooting, vecRight, vecUp;
		math::AngleVectors(viewangles, &vecDirShooting, &vecRight, &vecUp);

		Vector vecDir = vecDirShooting + (vecRight * spreads_x.at(iBullet)) + (vecUp * spreads_y.at(iBullet));
		vecDir.NormalizeInPlace();

		Vector vecEnd = vecStart + vecDir * weapon_data->Range;

		Ray_t ray;
		ray.Init(vecStart, vecEnd);

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

		static auto DrawClientHitboxes = reinterpret_cast<void(__thiscall*)(void*, float, bool)>(tools::find_pattern("client.dll", "55 8B EC 83 EC 60 57"));

		DrawClientHitboxes(tr.m_pEnt, 4.f, false);
		g_debugoverlay->AddBoxOverlay(tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 255, 0, 127, 4.f);
	}
}

void __stdcall _CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	oCreateMove(sequence_number, input_sample_frametime, active);

	auto cmd = &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	auto verified = &g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

	if (!cmd || !verified)
		return;

	drawmanager->clear();

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (local)
	{
		aimbot->move(cmd);
		misc->move(cmd);
		visuals->render();

		if (GetAsyncKeyState(5))
		{
			auto weapon = local->GetActiveWeapon();
			if (weapon)
			{
				bruteforce_triggerbot(cmd, local, weapon);
			}
		}
	}

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();
}

void __declspec(naked) __stdcall CreateMove(int sequence_number, float input_sample_frametime, bool active)
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
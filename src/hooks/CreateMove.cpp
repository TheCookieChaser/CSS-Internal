#include "hooks.h"
#include "../tools/entity.h"
#include "../tools/math.h"
#include "../tools/config.h"
#include "../hacks/aimbot.h"
#include "../hacks/misc.h"

float RandomFloat(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat");
	return m_RandomFloat(min, max);
}

void RandomSeed(UINT seed)
{
	typedef void(*RandomSeed_t)(UINT);
	static RandomSeed_t m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed");
	m_RandomSeed(seed);
	return;
}

void VectorAngles(const Vector &vecForward, Vector &vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else
	{
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI;

		if (vecView[1] < 0.f)
			vecView[1] += 360.f;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI;
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}

void triggerbot(CUserCmd* cmd, C_CSPlayer* local, C_BaseCombatWeapon* weapon)
{
	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	RandomSeed((cmd->random_seed & 0xFF) + 1);

	auto v39 = RandomFloat(0.0, 6.2831855);
	auto v21 = RandomFloat(0.0, weapon->GetInaccuracy());
	auto v37 = cos(v39) * v21;
	auto v38 = v21 * sin(v39);

	float v23, v24;
	float v32[16];
	float v33[16];

	Vector vecStart = local->get_eye_position();

	weapon->UpdateAccuracyPenalty();

	auto viewangles = cmd->viewangles;
	viewangles += local->get_aim_punch() * 2.f;

	for (auto iBullet = 0; iBullet < weapon_data->Bullets; ++iBullet)
	{
		v39 = RandomFloat(0.0, 6.2831855);
		v23 = RandomFloat(0.0, weapon->GetSpread());
		v24 = v39;
		v32[iBullet] = cos(v39) * v23;
		v33[iBullet] = v23 * sin(v24);

		auto x = v32[iBullet] + v37;
		auto y = v33[iBullet] + v38;

		Vector vecDirShooting, vecRight, vecUp;
		math::AngleVectors(viewangles, &vecDirShooting, &vecRight, &vecUp);

		Vector vecDir = vecDirShooting + (vecRight * x) + (vecUp * y);
		vecDir.NormalizeInPlace();

		Vector vecEnd = vecStart + vecDir * 10000;

		Ray_t ray;
		ray.Init(vecStart, vecEnd);

		CTraceFilter filter;
		filter.pSkip = local;

		trace_t tr;

		g_trace->TraceRay(ray, 0x4600400B, &filter, &tr);

		g_debugoverlay->AddBoxOverlay(tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 255, 0, 127, -1.f);

		auto server_time = local->get_tick_base() * g_globalvars->interval_per_tick;
		if (tr.m_pEnt && tr.hitgroup > 0 && tr.hitgroup <= 20 && server_time > weapon->GetNextPrimaryAttack())
		{
			cmd->buttons |= IN_ATTACK;

			static auto DrawClientHitboxes = reinterpret_cast<void(__thiscall*)(void*, float, bool)>(tools::find_pattern("client.dll", "55 8B EC 83 EC 60 57"));

			DrawClientHitboxes(tr.m_pEnt, 4.f, false);
			g_debugoverlay->AddBoxOverlay(tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 255, 0, 127, 4.f);
		}
	}
}

void __stdcall _CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	oCreateMove(sequence_number, input_sample_frametime, active);

	auto cmd = &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	auto verified = &g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

	if (!cmd || !verified)
		return;

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (local)
	{
		aimbot->move(cmd);
		misc->move(cmd);

		if (GetAsyncKeyState(5))
		{
			auto weapon = local->GetActiveWeapon();
			if (weapon)
			{
				triggerbot(cmd, local, weapon);
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
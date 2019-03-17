#include "hooks.h"
#include "../tools/entity.h"
#include "../tools/math.h"
#include "../tools/config.h"
#include "../tools/game.h"
#include "../hacks/aimbot.h"
#include "../hacks/visuals.h"
#include "../hacks/misc.h"

void engine_prediction(C_BasePlayer* local_player, CUserCmd* cmd)
{
	static CMoveData* g_pMoveData = nullptr;
	if (!g_pMoveData)
		g_pMoveData = static_cast<CMoveData*>(malloc(sizeof CMoveData));

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

	//const auto v2 = cmd->buttons ^ *reinterpret_cast<std::uintptr_t *>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1018);
	//*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x100C)
	//= *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1018);
	//*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1018) = cmd->buttons;
	//*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1010) = cmd->buttons & v2;
	//*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(local_player) + 0x1014) = v2 & ~cmd->buttons;
	local_player->UpdateButtonState(cmd->buttons);

	local_player->SetLocalViewAngles(cmd->viewangles);

	memset(g_pMoveData, 0, sizeof(CMoveData));
	g_prediction->SetupMove(local_player, cmd, g_movehelper, g_pMoveData);
	g_gamemovement->ProcessMovement(local_player, g_pMoveData);// WHY THE FUCK????????????????????????????
	g_prediction->FinishMove(local_player, cmd, g_pMoveData);
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

	auto local_player = reinterpret_cast<C_BasePlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (local_player)
	{
		misc->autojump(cmd);

		//ghetto engine prediction since something is wrong my own
		{
			const auto original_tickbase = local_player->get_tick_base();
			g_prediction->RunCommand(local_player, cmd, g_movehelper);
			local_player->get_tick_base() = original_tickbase;
		}
		//engine_prediction(local_player, cmd);

		misc->move(cmd);
		aimbot->move(cmd, sendpacket);
		visuals->render();
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
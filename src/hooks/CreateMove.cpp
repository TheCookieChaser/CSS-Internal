#include "hooks.h"
#include "../tools/entity.h"
#include "../tools/math.h"
#include "../tools/config.h"
#include "../tools/game.h"
#include "../hacks/aimbot.h"
#include "../hacks/visuals.h"
#include "../hacks/misc.h"

void __stdcall _CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	o_create_move(sequence_number, input_sample_frametime, active);

	auto cmd = &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	auto verified = &g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

	if (!cmd || !verified)
		return;

	auto local_player = reinterpret_cast<C_BasePlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (local_player)
	{
		misc->autojump(cmd);

		misc->engine_prediction(local_player, cmd);

		misc->move(cmd);
		aimbot->move(cmd, sendpacket);
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
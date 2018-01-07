#include "Hooks.h"
#include "SDK\SDK.h"
#include "Math.h"
#include "Entity.h"
#include "Settings.h"
#include "Aimbot.h"
#include "Misc.h"

void __stdcall CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	oCreateMove(sequence_number, input_sample_frametime, active);

	auto cmd = &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	auto verified = &g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

	if (!cmd || !verified)
		return;

	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (local)
	{
		Aimbot::CreateMove(cmd);
		Misc::CreateMove(cmd);
	}

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();
}

void __declspec(naked) __stdcall CreateMove_Hooked(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		LEA  EAX, [EBP - 01]
		PUSH EAX
		PUSH[ESP + 0x10]
		PUSH[ESP + 0x10]
		PUSH[ESP + 0x10]
		CALL CreateMove
		RET 0x0C
	}
}
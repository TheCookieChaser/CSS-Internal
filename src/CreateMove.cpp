#include "Hooks.h"
#include "SDK\SDK.h"
#include "Math.h"
#include "Entity.h"
#include "Settings.h"
#include "Aimbot.h"
#include "Misc.h"

void __stdcall CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket);

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

void __stdcall CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	ClientTable->GetOriginal<CreateMove_t>(21)(sequence_number, input_sample_frametime, active);

	CUserCmd *pCmd = &input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	CVerifiedUserCmd *pVerified = &input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

	if (!pCmd || !pVerified)
		return;

	auto pLocal = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));

	if (!pLocal)
		return;

	Aimbot::CreateMove(pCmd);
	Misc::CreateMove(pCmd);

	pVerified->m_cmd = *pCmd;
	pVerified->m_crc = pCmd->GetChecksum();
}
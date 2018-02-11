#include "hooks.h"

CUserCmd* __fastcall get_user_cmd(void* thisptr, void*, int sequence_number)
{
	return &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}
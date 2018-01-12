#include "Hooks.h"

#define	MAX_EDICT_BITS 11
#define WEAPON_SUBTYPE_BITS	6

void ValidateUserCmd(CUserCmd *usercmd, int sequence_number)
{
	CRC32_t crc = usercmd->GetChecksum();
	if (crc != g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP].m_crc)
	{
		*usercmd = g_input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP].m_cmd;
	}
}

CUserCmd* GetUserCmd(int sequence_number)
{
	return  &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];;
}

bool __fastcall WriteUsercmdDeltaToBuffer(void* ecx, void* edx, void* buf, int from, int to, bool isnewcommand)
{
	CUserCmd nullcmd;
	CUserCmd *v7, *v8, *v9, *v33;

	if (from == -1)
	{
		v7 = &nullcmd;
	}
	else
	{
		v8 = GetUserCmd(from);
		v7 = v8;

		if (v8)
			ValidateUserCmd(v8, from);
		else
			v7 = &nullcmd;
	}

	v9 = GetUserCmd(to);
	v33 = v9;

	if (v9)
		ValidateUserCmd(v9, to);
	else
		v9 = &nullcmd;

	static auto WriteUsercmd = reinterpret_cast<void(__cdecl*)(void* buffer, CUserCmd* from, CUserCmd* to)>(
		tools::find_pattern("client.dll", "55 8B EC 8B 45 10 83 EC 08"));

	WriteUsercmd(buf, v9, v7);

	return !(*reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(buf) + 0x10));
}
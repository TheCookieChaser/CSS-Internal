#include "hooks.h"

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
	return &g_input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];;
}

bool __fastcall write_usercmd_delta_to_buffer(void* thisptr, void*, void* buf, int from, int to, bool isnewcommand)
{
	static auto WriteUsercmd = reinterpret_cast<void(__cdecl*)(void*, CUserCmd*, CUserCmd*)>(
		tools::find_pattern("client.dll", "55 8B EC 8B 45 10 83 EC 08"));

	CUserCmd* t, *f;
	CUserCmd nullcmd;

	if (from == -1)
	{
		f = &nullcmd;
	}
	else
	{
		f = GetUserCmd(from);
		if (f)
		{
			ValidateUserCmd(f, from);
		}
		else
		{
			// DevMsg( "WARNING! User command delta too old (from %i, to %i)\n", from, to );
			f = &nullcmd;
		}
	}

	t = GetUserCmd(to);
	if (t)
	{
		ValidateUserCmd(t, to);
	}
	else
	{
		t = &nullcmd;
	}

	WriteUsercmd(buf, t, f);

	return *reinterpret_cast<BYTE*>(reinterpret_cast<std::uintptr_t>(buf) + 0x10) == 0;
}
#pragma once

#define MULTIPLAYER_BACKUP 90

class CUserCmd
{
public:
	virtual ~CUserCmd() { };

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

		CRC32_Final(&crc);
		return crc;
	}

	int command_number; //0x0004 
	int tick_count; //0x0008 
	Vector viewangles; //0x000C 
	float forwardmove; //0x0018 
	float sidemove; //0x001C 
	float upmove; //0x0020 
	int buttons; //0x0024 
	byte impulse; //0x0028 
	int weaponselect; //0x002C 
	int weaponsubtype; //0x0030 
	int random_seed; //0x0034 
	short mousedx; //0x0038 
	short mousedy; //0x003C 
	bool hasbeenpredicted; //0x0040 

}; //Size=0x0044

class CVerifiedUserCmd
{
public:
	CUserCmd  m_cmd;
	CRC32_t   m_crc;
};

class CInput
{
public:
	char pad_0x0000[0x8C]; //0x0000
	unsigned char N00000024; //0x008C 
	unsigned char m_fCameraInThirdPerson; //0x008D 
	char pad_0x008E[0x16]; //0x008E
	Vector m_angPreviousViewAngles; //0x00A4 
	float m_flLastForwardMove; //0x00B0 
	char pad_0x00B4[0x10]; //0x00B4
	CUserCmd* m_pCommands; //0x00C4 
	CVerifiedUserCmd * m_pVerifiedCommands; //0x00C8 

}; //Size=0x00CC
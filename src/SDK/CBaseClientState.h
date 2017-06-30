#pragma once

class CBaseClientState
{
public:
	char pad_0x0000[0xC]; //0x0000
	float m_flConnectTime; //0x000C 
	__int32 m_nRetryNumber; //0x0010 
	char pad_0x0014[0x4]; //0x0014
	__int32 N00000007; //0x0018 
	char pad_0x001C[0x4]; //0x001C
	__int32 N00000009; //0x0020 
	char pad_0x0024[0x40]; //0x0024
	__int32 m_nCurrentSequence; //0x0064 
	char pad_0x0068[0x8]; //0x0068
	__int32 m_nDeltaTick; //0x0070 
	__int32 m_bPaused; //0x0074 
	__int32 m_nViewEntity; //0x0078 
	__int32 m_nPlayerSlot; //0x007C 
	char m_szLevelName[40]; //0xB37888 
	char pad_0x00A8[0x57]; //0x00A8
	char m_szLevelNameShort[40]; //0xB37888 
	char pad_0x0127[0x59]; //0x0127
	__int32 m_nMaxClients; //0x0180 
	char pad_0x0184[0x55C]; //0x0184

}; //Size=0x06E0
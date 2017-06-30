#pragma once

#define MAX_PLAYER_NAME_LENGTH 32
#define SIGNED_GUID_LEN 32

typedef struct player_info_s 
{
	char szName[128];
	int userid;
	int m_nUserID;
	char guid[SIGNED_GUID_LEN + 1];
	unsigned int friendsid;
	char friendsname[MAX_PLAYER_NAME_LENGTH + 96];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
} player_info_t;
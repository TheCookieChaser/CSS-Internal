#pragma once

#include "SDK.h"
#include "../NetVarManager.h"
#include "../virtuals.h"

class C_BaseCombatWeapon;
class C_BaseEntity : public IClientEntity
{
public:
	NETVAR(get_team_num, "CBaseEntity", "m_iTeamNum", std::int32_t);
	NETVAR(get_origin, "CBaseEntity", "m_vecOrigin", Vector);
};

class C_BaseAnimating : public C_BaseEntity
{
public:
	NETVAR(get_hitbox_set, "CBaseAnimating", "m_nHitboxSet", std::uint32_t);
};

class C_BasePlayer : public C_BaseAnimating
{
public:
	NETVAR(get_life_state, "CBasePlayer", "m_lifeState", std::uint8_t);
	NETVAR(get_health, "CBasePlayer", "m_iHealth", std::int32_t);
	NETVAR(get_tick_base, "CBasePlayer", "m_nTickBase", std::uint32_t);
	NETVAR(get_aim_punch, "CBasePlayer", "m_vecPunchAngle", Vector);
	NETVAR(get_view_offset, "CBasePlayer", "m_vecViewOffset[0]", Vector);
};

class C_CSPlayer : public C_BasePlayer
{
public:
	NETVAR(get_flags, "CCSPlayer", "m_fFlags", std::int32_t);
	NETVAR(get_eye_angles, "CCSPlayer", "m_angEyeAngles[0]", Vector);

	Vector get_eye_position() 
	{
		return get_origin() + get_view_offset();;
	}

	C_BaseCombatWeapon* GetActiveWeapon()
	{
		return get_vfunc<C_BaseCombatWeapon*(__thiscall*)(void*)>(this, 222)(this);
	}
};

class FileWeaponInfo_t
{
public:
	char pad_0x0000[0x6]; //0x0000
	char name[64]; //0x1037888 
	char pad_0x0046[0x10]; //0x0046
	char valamigeci[64]; //0x1037888 
	char pad_0x0096[0x10]; //0x0096
	char viewmodelname[64]; //0x1037888 
	char pad_0x00E6[0x10]; //0x00E6
	char worldmodelname[64]; //0x1037888 
	char pad_0x0136[0x4A]; //0x0136
	char bullettype[64]; //0x1037888 
	char pad_0x01C0[0x50]; //0x01C0
	char masiknev[64]; //0x1037888 
	char pad_0x0250[0x380]; //0x0250
	char zoomname[32]; //0x1037888 
	char pad_0x05F0[0x294]; //0x05F0
	__int32 penetration; //0x0884 
	__int32 damage; //0x0888 
	float range; //0x088C 
	float rangemodifier; //0x0890 
	__int32 bullets; //0x0894 
	float cycletime; //0x0898 
	char pad_0x089C[0xEC]; //0x089C

}; //Size=0x0988

class C_BaseCombatWeapon : public C_BaseEntity
{
public:
	float GetNextPrimaryAttack()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + 0x878);
	}

	int Clip1()
	{
		return get_vfunc<int(__thiscall*)(void*)>(this, 320)(this);
	}

	int GetWeaponID()
	{
		return get_vfunc<int(__thiscall*)(void*)>(this, 365)(this);
	}

	bool IsFullAuto()
	{
		return get_vfunc<bool(__thiscall*)(void*)>(this, 363)(this);
	}

	float GetInaccuracy()
	{
		return get_vfunc<float(__thiscall*)(void*)>(this, 377)(this);
	}

	float GetSpread()
	{
		return get_vfunc<float(__thiscall*)(void*)>(this, 376)(this);
	}

	void UpdateAccuracyPenalty()
	{
		return get_vfunc<void(__thiscall*)(void*)>(this, 378)(this);
	}

	float& GetAccuracyPenalty()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + 0x930);
	}

	FileWeaponInfo_t& GetWeaponData()
	{
		FileWeaponInfo_t a;
		if (!this)
			return a;

		static uint64_t GetWeaponData;
		if (!GetWeaponData)
			GetWeaponData = Tools::FindSignature("client.dll", "0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3");
		
		return reinterpret_cast<FileWeaponInfo_t&(__thiscall*)(C_BaseCombatWeapon*)>(GetWeaponData)(this);
	}
};
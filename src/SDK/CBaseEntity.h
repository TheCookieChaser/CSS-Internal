#pragma once

#include "SDK.h"
#include "../NetVarManager.h"

class C_BaseCombatWeapon;

class C_BaseEntity : public IClientEntity
{
public:
	int GetTeamNum()
	{
		static int m_iTeamNum = NetVarManager->GetOffset("DT_BaseEntity", "m_iTeamNum");
		return *reinterpret_cast<int*>((uintptr_t)this + m_iTeamNum);
	}

	Vector GetVecOrigin()
	{
		static int m_vecOrigin = NetVarManager->GetOffset("DT_BaseEntity", "m_vecOrigin");
		return *reinterpret_cast<Vector*>((uintptr_t)this + m_vecOrigin);
	}
};

class C_BaseAnimating : public C_BaseEntity
{
public:
	int GetHitboxSet()
	{
		static int m_nHitboxSet = NetVarManager->GetOffset("DT_BaseAnimating", "m_nHitboxSet");
		return *reinterpret_cast<int*>((uintptr_t)this + m_nHitboxSet);
	}
};

class C_BasePlayer : public C_BaseAnimating
{
public:
	unsigned char GetLifeState()
	{
		static int m_lifeState = NetVarManager->GetOffset("DT_BasePlayer", "m_lifeState");
		return *reinterpret_cast<unsigned char*>((uintptr_t)this + m_lifeState);
	}

	int GetHealth()
	{
		static int m_iHealth = NetVarManager->GetOffset("DT_BasePlayer", "m_iHealth");
		return *reinterpret_cast<int*>((uintptr_t)this + m_iHealth);
	}

	int GetTickBase()
	{
		static int m_nTickBase = NetVarManager->GetOffset("DT_BasePlayer", "m_nTickBase");
		return *reinterpret_cast<int*>((uintptr_t)this + m_nTickBase);
	}

	Vector* GetAimPunch()
	{
		static int m_vecPunchAngle = NetVarManager->GetOffset("DT_BasePlayer", "m_vecPunchAngle");
		return reinterpret_cast<Vector*>((uintptr_t)this + m_vecPunchAngle);
	}

	Vector GetVecViewOffset()
	{
		static int m_vecViewOffset = NetVarManager->GetOffset("DT_BasePlayer", "m_vecViewOffset[0]");
		return *reinterpret_cast<Vector*>((uintptr_t)this + m_vecViewOffset);
	}
};

class C_CSPlayer : public C_BasePlayer
{
public:
	C_BaseCombatWeapon* GetActiveWeapon()
	{
		C_BaseCombatWeapon* pActiveWeapon;
		__asm
		{
			MOV ECX, this
			MOV EAX, [ECX]
			CALL DWORD PTR DS : [EAX + 0x378]
			MOV pActiveWeapon, EAX
		}
		return pActiveWeapon;
	}

	int GetFlags()
	{
		static int m_fFlags = NetVarManager->GetOffset("DT_CSPlayer", "m_fFlags");
		return *reinterpret_cast<int*>((uintptr_t)this + m_fFlags);
	}

	Vector GetEyePosition()
	{
		return GetVecOrigin() + GetVecViewOffset();
	}

	Vector* GetEyeAngles()
	{
		static int m_angEyeAngles = NetVarManager->GetOffset("DT_CSPlayer", "m_angEyeAngles[0]");
		return reinterpret_cast<Vector*>((uintptr_t)this + m_angEyeAngles);
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
		if (!this)
			return 0.f;

		return *(float*)((DWORD)this + 0x878);
	}

	int Clip1()
	{
		if (!this)
			return 0;

		typedef int(__thiscall* GetSubType_t)(void*);
		return CallVFunction<GetSubType_t>(this, 320)(this);
	}

	int GetWeaponID()
	{
		if (!this)
			return 0;

		typedef int(__thiscall* GetWeaponID_t)(void*);
		return CallVFunction<GetWeaponID_t>(this, 365)(this);
	}

	bool IsFullAuto()
	{
		if (!this)
			return 0.f;

		typedef bool(__thiscall* IsFullAuto_t)(void*);
		return CallVFunction<IsFullAuto_t>(this, 363)(this);
	}

	float GetInaccuracy()
	{
		if (!this)
			return 0.f;

		typedef float(__thiscall* GetInaccuracy_t)(void*);
		return CallVFunction<GetInaccuracy_t>(this, 377)(this);
	}

	float GetSpread()
	{
		if (!this)
			return 0.f;

		typedef float(__thiscall* GetSpread_t)(void*);
		return CallVFunction<GetSpread_t>(this, 376)(this);
	}

	void UpdateAccuracyPenalty()
	{
		if (!this)
			return;

		typedef void(__thiscall* UpdateAccuracyPenalty_t)(void*);
		return CallVFunction<UpdateAccuracyPenalty_t>(this, 378)(this);
	}

	float& GetAccuracyPenalty()
	{
		float a = 0.0f;
		if (!this)
			return a;

		return *(float*)((DWORD)this + 0x930);
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
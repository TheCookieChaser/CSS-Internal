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

class C_BaseCombatWeapon : public C_BaseEntity
{
public:
	float GetNextPrimaryAttack()
	{
		return *(float*)((DWORD)this + 0x878);
	}

	int Clip1()
	{
		typedef int(__thiscall* GetSubType_t)(void*);
		return CallVFunction<GetSubType_t>(this, 320)(this);
	}

	int GetWeaponID()
	{
		typedef int(__thiscall* GetWeaponID_t)(void*);
		return CallVFunction<GetWeaponID_t>(this, 365)(this);
	}

	const char* GetName()
	{
		typedef const char*(__thiscall* GetName_t)(void*);
		return CallVFunction<GetName_t>(this, 311)(this);
	}

	bool IsFullAuto()
	{
		typedef bool(__thiscall* IsFullAuto_t)(void*);
		return CallVFunction<IsFullAuto_t>(this, 363)(this);
	}

	float GetInaccuracy()
	{
		typedef float(__thiscall* GetInaccuracy_t)(void*);
		return CallVFunction<GetInaccuracy_t>(this, 377)(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall* GetSpread_t)(void*);
		return CallVFunction<GetSpread_t>(this, 376)(this);
	}

	void UpdateAccuracyPenalty()
	{
		typedef void(__thiscall* UpdateAccuracyPenalty_t)(void*);
		return CallVFunction<UpdateAccuracyPenalty_t>(this, 378)(this);
	}

	float& GetAccuracyPenalty()
	{
		return *(float*)((DWORD)this + 0x930);
	}
};
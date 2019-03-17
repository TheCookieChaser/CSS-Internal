#pragma once

#include "SDK.h"
#include "../tools/netvar_manager.h"
#include "../tools/virtuals.h"
#include "../tools/tools.h"

class C_BaseCombatWeapon;
class C_BaseEntity : public IClientEntity
{
public:
	NETVAR(get_team_num, "CBaseEntity", "m_iTeamNum", std::int32_t);
	NETVAR(get_origin, "CBaseEntity", "m_vecOrigin", Vector);

	void SetAbsOrigin(const Vector& a2)
	{
		static auto set_abs_origin = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(
			tools::find_pattern("client.dll", "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08"));
		set_abs_origin(this, a2);
	}
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
	NETVAR(get_flags, "CBasePlayer", "m_fFlags", std::int32_t);
	NETVAR(get_tick_base, "CBasePlayer", "m_nTickBase", std::uint32_t);
	NETVAR(get_aim_punch, "CBasePlayer", "m_vecPunchAngle", Vector);
	NETVAR(get_view_offset, "CBasePlayer", "m_vecViewOffset[0]", Vector);
	NETVAR(get_next_attack, "CBaseCombatCharacter", "m_flNextAttack", float);

	C_BaseCombatWeapon* GetActiveWeapon()
	{
		return get_vfunc<C_BaseCombatWeapon*(__thiscall*)(void*)>(this, 222)(this);
	}

	void SetLocalViewAngles(const QAngle &viewAngles)
	{
		get_vfunc<void(__thiscall*)(void*, const QAngle &)>(this, 293)(this, viewAngles);
	}

	const Vector& WorldSpaceCenter()
	{
		return get_vfunc<Vector&(__thiscall*)(void*)>(this, 66)(this);
	}

	Vector get_eye_position()
	{
		return get_origin() + get_view_offset();
	}

	void UpdateButtonState(int nUserCmdButtonMask)
	{
		static auto function = reinterpret_cast<void(__thiscall*)(void*, int)>(
			tools::get_rel32(tools::find_pattern("client.dll", "E8 ? ? ? ? 8B 17 8D 43 0C"), 1, 5));
		function(this, nUserCmdButtonMask);
	}
};

class C_CSPlayer : public C_BasePlayer
{
public:
	NETVAR(get_eye_angles, "CCSPlayer", "m_angEyeAngles[0]", Vector);
	NETVAR(get_armor_value, "CCSPlayer", "m_ArmorValue", int);
	NETVAR(get_has_helmet, "CCSPlayer", "m_bHasHelmet", bool);
};

#define MAX_WEAPON_STRING	80
#define MAX_WEAPON_PREFIX	16
#define MAX_WEAPON_AMMO_NAME		32

typedef enum {
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE, // Can't be "DOUBLE" because windows.h uses it.
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	DEPLOY,

	// Add new shoot sound types here

	NUM_SHOOT_SOUND_TYPES,
} WeaponSound_t;

class CHudTexture;

#pragma pack (push, 1)

class CCSWeaponInfo
{
public:
	virtual void Parse(KeyValues*, const char*) = 0;

	bool bParsedScript; //0x0004
	bool bLoadedHudElements; //0x0005
	char szClassName[MAX_WEAPON_STRING]; //0x0006
	char szPrintName[MAX_WEAPON_STRING]; //0x0056
	char szViewModel[MAX_WEAPON_STRING]; //0x00A6
	char szWorldModel[MAX_WEAPON_STRING]; //0x00F6
	char szAnimationPrefix[MAX_WEAPON_PREFIX]; //0x0146
	char pad_0156[2]; //0x0156
	int32_t iSlot; //0x0158
	int32_t iPosition; //0x015C
	int32_t iMaxClip1; //0x0160
	int32_t iMaxClip2; //0x0164
	int32_t iDefaultClip1; //0x0168
	int32_t iDefaultClip2; //0x016C
	int32_t iWeight; //0x0170
	int32_t iRumbleEffect; //0x0174
	bool bAutoSwitchTo; //0x0178
	bool bAutoSwitchFrom; //0x0179
	char pad_017A[2]; //0x017A
	int32_t iFlags; //0x017C
	char szAmmo1[MAX_WEAPON_AMMO_NAME]; //0x0180
	char szAmmo2[MAX_WEAPON_AMMO_NAME]; //0x01A0
	char aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING]; //0x01C0
	int32_t iAmmoType; //0x06C0
	int32_t iAmmo2Type; //0x06C4
	bool m_bMeleeWeapon; //0x06C8
	bool m_bBuiltRightHanded; //0x06C9
	bool m_bAllowFlipping; //0x06CA
	char pad_06CB[1]; //0x06CB
	int32_t iSpriteCount; //0x06CC
	class CHudTexture* iconActive; //0x06D0
	class CHudTexture* iconInactive; //0x06D4
	class CHudTexture* iconAmmo; //0x06D8
	class CHudTexture* iconAmmo2; //0x06DC
	class CHudTexture* iconCrosshair; //0x06E0
	class CHudTexture* iconAutoaim; //0x06E4
	class CHudTexture* iconZoomedCrosshair; //0x06E8
	class CHudTexture* iconZoomedAutoaim; //0x06EC
	class CHudTexture* iconSmall; //0x06F0
	bool bShowUsageHint; //0x06F4
	char pad_06F5[3]; //0x06F5
	float m_flMaxSpeed; //0x06F8
	int32_t m_WeaponType; //0x06FC
	bool bFullAuto; //0x0700
	char pad_0701[3]; //0x0701
	int32_t m_iTeam; //0x0704
	float m_flBotAudibleRange; //0x0708
	float m_flArmorRatio; //0x070C
	int32_t m_iCrosshairMinDistance; //0x0710
	int32_t m_iCrosshairDeltaDistance; //0x0714
	bool m_bCanUseWithShield; //0x0718
	char m_WrongTeamMsg[32]; //0x0719
	char m_szAnimExtension[16]; //0x0739
	char m_szShieldViewModel[64]; //0x0749
	char m_szAddonModel[MAX_WEAPON_STRING]; //0x0789
	char m_szDroppedModel[MAX_WEAPON_STRING]; //0x07D9
	char m_szSilencerModel[MAX_WEAPON_STRING]; //0x0829
	char pad_0879[3]; //0x0879
	int32_t m_iMuzzleFlashStyle; //0x087C
	float m_flMuzzleScale; //0x0880
	int32_t m_iPenetration; //0x0884
	int32_t m_iDamage; //0x0888
	float m_flRange; //0x088C
	float m_flRangeModifier; //0x0890
	int32_t m_iBullets; //0x0894
	float m_flCycleTime; //0x0898
	bool m_bAccuracyQuadratic; //0x089C
	char pad_089D[3]; //0x089D
	float m_flAccuracyDivisor; //0x08A0
	float m_flAccuracyOffset; //0x08A4
	float m_flMaxInaccuracy; //0x08A8
	float m_flSpread; //0x08AC
	float m_flSpreadAlt; //0x08B0
	float m_flInaccuracyCrouch; //0x08B4
	float m_flInaccuracyCrouchAlt; //0x08B8
	float m_flInaccuracyStand; //0x08BC
	float m_flInaccuracyStandAlt; //0x08C0
	float m_flInaccuracyJump; //0x08C4
	float m_flInaccuracyJumpAlt; //0x08C8
	float m_flInaccuracyLand; //0x08CC
	float m_flInaccuracyLandAlt; //0x08D0
	float m_flInaccuracyLadder; //0x08D4
	float m_flInaccuracyLadderAlt; //0x08D8
	float m_flInaccuracyFire; //0x08DC
	float m_flInaccuracyFireAlt; //0x08E0
	float m_flInaccuracyMove; //0x08E4
	float m_flInaccuracyMoveAlt; //0x08E8
	float m_flRecoveryTimeStand; //0x08EC
	float m_flRecoveryTimeCrouch; //0x08F0
	float m_flInaccuracyReload; //0x08F4
	float m_flInaccuracyAltSwitch; //0x08F8
	float m_flTimeToIdleAfterFire; //0x08FC
	float m_flIdleInterval; //0x0900
	int32_t m_iWeaponPrice; //0x0904
	int32_t m_iDefaultPrice; //0x0908
	int32_t m_iPreviousPrice; //0x090C
}; //Size: 0x0910

#pragma pack(pop)

class C_BaseCombatWeapon : public C_BaseEntity
{
public:
	NETVAR(get_next_primary_attack, "CBaseCombatWeapon", "m_flNextPrimaryAttack", float);

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
		return get_vfunc<float(__thiscall*)(void*)>(this, 376)(this);
	}

	float GetSpread()
	{
		return get_vfunc<float(__thiscall*)(void*)>(this, 377)(this);
	}

	void UpdateAccuracyPenalty()
	{
		return get_vfunc<void(__thiscall*)(void*)>(this, 378)(this);
	}

	float& GetAccuracyPenalty()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + 0x930);
	}

	CCSWeaponInfo* GetWpnData()
	{
		static uint64_t oGetWpnData = tools::find_pattern("client.dll", "0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3");
		return reinterpret_cast<CCSWeaponInfo*(__thiscall*)(C_BaseCombatWeapon*)>(oGetWpnData)(this);
	}
};
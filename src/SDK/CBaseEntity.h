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
};

class C_CSPlayer : public C_BasePlayer
{
public:
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

class CCSWeaponInfo
{
public:
	virtual void Parse(KeyValues*, const char*) = 0;

	char pad_0004[2]; //0x0004
	char weaponname[80]; //0x0006
	char printname[80]; //0x0056
	char viewmodel[80]; //0x00A6
	char playermodel[80]; //0x00F6
	char anim_prefix[16]; //0x0146
	char pad_0156[2]; //0x0156
	int32_t bucket; //0x0158
	int32_t bucket_position; //0x015C
	int32_t clip_size; //0x0160
	int32_t clip2_size; //0x0164
	int32_t default_clip; //0x0168
	int32_t default_clip2; //0x016C
	int32_t weight; //0x0170
	int32_t rumble; //0x0174
	bool autoswitchto; //0x0178
	bool autoswitchfrom; //0x0179
	char pad_017A[2]; //0x017A
	int32_t item_flags; //0x017C
	char primary_ammo[32]; //0x0180
	char secondary_ammo[32]; //0x01A0
	char SoundData[80]; //0x01C0
	char pad_0210[1208]; //0x0210
	bool MeleeWeapon; //0x06C8
	bool BuiltRightHanded; //0x06C9
	bool AllowFlipping; //0x06CA
	char pad_06CB[41]; //0x06CB
	bool showusagehint; //0x06F4
	char pad_06F5[3]; //0x06F5
	float MaxPlayerSpeed; //0x06F8
	char pad_06FC[4]; //0x06FC
	bool FullAuto; //0x0700
	char pad_0701[3]; //0x0701
	int32_t Team; //0x0704
	float BotAudibleRange; //0x0708
	float WeaponArmorRatio; //0x070C
	int32_t CrosshairMinDistance; //0x0710
	int32_t CrosshairDeltaDistance; //0x0714
	int8_t CanEquipWithShield; //0x0718
	char WrongTeamMsg[32]; //0x0719
	char PlayerAnimationExtension[16]; //0x0739
	char shieldviewmodel[64]; //0x0749
	char AddonModel[80]; //0x0789
	char DroppedModel[80]; //0x07D9
	char SilencerModel[80]; //0x0829
	char pad_0879[3]; //0x0879
	int32_t MuzzleFlashStyle; //0x087C
	float MuzzleFlashScale; //0x0880
	int32_t Penetration; //0x0884
	int32_t Damage; //0x0888
	float Range; //0x088C
	float RangeModifier; //0x0890
	int32_t Bullets; //0x0894
	float CycleTime; //0x0898
	uint8_t AccuracyQuadratic; //0x089C
	char pad_089D[3]; //0x089D
	float AccuracyDivisor; //0x08A0
	float AccuracyOffset; //0x08A4
	float MaxInaccuracy; //0x08A8
	float Spread; //0x08AC
	float SpreadAlt; //0x08B0
	float InaccuracyCrouch; //0x08B4
	float InaccuracyCrouchAlt; //0x08B8
	float InaccuracyStand; //0x08BC
	float InaccuracyStandAlt; //0x08C0
	float InaccuracyJump; //0x08C4
	float InaccuracyJumpAlt; //0x08C8
	float InaccuracyLand; //0x08CC
	float InaccuracyLandAlt; //0x08D0
	float InaccuracyLadder; //0x08D4
	float InaccuracyLadderAlt; //0x08D8
	float InaccuracyFire; //0x08DC
	float InaccuracyFireAlt; //0x08E0
	float InaccuracyMove; //0x08E4
	float InaccuracyMoveAlt; //0x08E8
	float RecoveryTimeStand; //0x08EC
	float RecoveryTimeCrouch; //0x08F0
	float InaccuracyReload; //0x08F4
	float InaccuracyAltSwitch; //0x08F8
	float TimeToIdle; //0x08FC
	float IdleInterval; //0x0900
	int32_t WeaponPrice; //0x0904
	int32_t WeaponPrice2; //0x0908

}; //Size: 0x090C

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
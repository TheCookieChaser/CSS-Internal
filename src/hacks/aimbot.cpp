#include "aimbot.h"
#include "../tools/game.h"

caimbot* aimbot = new caimbot();

void apply_nospread_angles(CUserCmd* cmd, C_BasePlayer* local_player, Vector& viewangles)
{
	//if (!(cmd->buttons & IN_ATTACK))
	//	return;

	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return;

	//const auto server_time = local_player->get_tick_base() * g_globals->interval_per_tick;
	//if (server_time <= weapon->get_next_primary_attack() || server_time <= local_player->get_next_attack())
	//	return;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	weapon->UpdateAccuracyPenalty();

	game::RandomSeed((cmd->random_seed & 0xFF) + 1);

	const auto rand1 = game::RandomFloat(0.f, M_PI_F * 2.f);
	const auto rand2 = game::RandomFloat(0.f, weapon->GetInaccuracy());

	const auto v37 = cosf(rand1) * rand2;
	const auto v38 = sinf(rand1) * rand2;

	auto vec_start = local_player->get_eye_position();

	const auto rand3 = game::RandomFloat(0.0, M_PI_F * 2.f);
	const auto rand4 = game::RandomFloat(0.0, weapon->GetSpread());

	const auto spread_x = cosf(rand3) * rand4 + v37;
	const auto spread_y = sinf(rand3) * rand4 + v38;

	Vector forward, right, up;
	math::AngleVectors(viewangles, &forward, &right, &up);

	auto vec_dir = forward + (right * spread_x * -1.f) + (up * spread_y * -1.f);
	VectorNormalize(vec_dir);

	//i have no fucking idea why but i have to negate this
	math::VectorAngles(-vec_dir, viewangles);

	math::clamp_angles(viewangles);
}

void GetBulletTypeParameters(int iBulletType, float &fPenetrationPower, float &flPenetrationDistance)
{
	static auto function = reinterpret_cast<void(__stdcall*)(int, float&, float&)>(
		tools::get_rel32(tools::find_pattern("client.dll", "E8 ? ? ? ? F3 0F 10 43 ? 33 FF"), 1, 5));
	function(iBulletType, fPenetrationPower, flPenetrationDistance);
}

//Yes i love copy pasting from ida (55 8B EC 8B 45 08 83 C0 BD)
void GetMaterialParameters(int iMaterial, float &flPenetrationModifier, float &flDamageModifier)
{
	switch (iMaterial)
	{
	case 67:
		flPenetrationModifier = 0.40000001;
		flDamageModifier = 0.25;
		break;
	case 68:
	case 77:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.30000001;
		break;
	case 71:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.99000001;
		break;
	case 80:
		flPenetrationModifier = 0.40000001;
		flDamageModifier = 0.44999999;
		break;
	case 84:
		flPenetrationModifier = 0.64999998;
		flDamageModifier = 0.30000001;
		break;
	case 86:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.44999999;
		break;
	case 87:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.60000002;
		break;
	default:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.5;
		break;
	}
}

static bool TraceToExit(Vector &start, Vector &dir, Vector &end, float flStepSize, float flMaxDistance)
{
	float flDistance = 0;
	Vector last = start;

	while (flDistance <= flMaxDistance)
	{
		flDistance += flStepSize;

		end = start + (dir * flDistance);

		if ((g_trace->GetPointContents(end) & MASK_SOLID) == 0)
		{
			// found first free point
			return true;
		}
	}

	return false;
}

bool IsArmored(int hitgroup, int armor_value, bool has_helmet)
{
	bool bApplyArmor = false;

	if (armor_value > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			bApplyArmor = true;
			break;
		case HITGROUP_HEAD:
			if (has_helmet)
			{
				bApplyArmor = true;
			}
			break;
		default:
			break;
		}
	}

	return bApplyArmor;
}

float scale_damage(float damage, int hitgroup, int armor_value, bool has_helmet, float weapon_armor_ratio)
{
	auto new_dmg = damage;
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		new_dmg *= 4.f;
		break;
	case HITGROUP_STOMACH:
		new_dmg *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		new_dmg *= 0.75f;
		break;
	default: 
		break;
	}

	auto armor_bonus = 0.5f;
	auto armor_ratio = weapon_armor_ratio * 0.5f;
	if (armor_value && IsArmored(hitgroup, armor_value, has_helmet))
	{
		auto flNew = new_dmg * armor_ratio;
		auto flArmor = (new_dmg - flNew) * armor_bonus;

		if (flArmor > armor_value)
		{
			flArmor = armor_value;
			flArmor *= (1 / armor_bonus);
			flNew = new_dmg - flArmor;
		}

		new_dmg = flNew;
	}

	return new_dmg;
}

bool can_penetrate_point(C_BasePlayer* local_player, C_BasePlayer* target_player, Vector shootAngles)
{
	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return false;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return false;

	auto iWeaponID = weapon->GetWeaponID();
	auto iDamage = weapon_data->m_iDamage;
	auto iBulletType = weapon_data->iAmmoType;
	auto flDistance = weapon_data->m_flRange;
	auto flRangeModifier = weapon_data->m_flRangeModifier;
	auto iPenetration = weapon_data->m_iPenetration;

	if (weapon->get_weapon_mode() == Secondary_Mode)
	{
		if (iWeaponID == WEAPON_GLOCK)
		{
			iDamage = 18;	// reduced power for burst shots
			flRangeModifier = 0.9f;
		}
		else if (iWeaponID == WEAPON_M4A1)
			flRangeModifier = 0.95f; // slower bullets in silenced mode
		else if (iWeaponID == WEAPON_USP)
			iDamage = 30; // reduced damage in silenced mode
	}

	float fCurrentDamage = iDamage;   // damage of the bullet at it's current trajectory
	float flCurrentDistance = 0.0;  //distance that the bullet has traveled so far

	Vector vecDir, vecRight, vecUp;
	math::AngleVectors(shootAngles, &vecDir, &vecRight, &vecUp);

	float flPenetrationPower = 0;		// thickness of a wall that this bullet can penetrate
	float flPenetrationDistance = 0;	// distance at which the bullet is capable of penetrating a wall
	float flDamageModifier = 0.5;		// default modification of bullets power after they go through a wall.
	float flPenetrationModifier = 1.f;

	GetBulletTypeParameters(iBulletType, flPenetrationPower, flPenetrationDistance);

	auto vecSrc = local_player->get_eye_position();

	while (fCurrentDamage > 0)
	{
		Vector vecEnd = vecSrc + vecDir * flDistance;

		trace_t tr;

		game::UTIL_TraceLine(vecSrc, vecEnd, CS_MASK_SHOOT | CONTENTS_HITBOX, local_player, 0, &tr);
		{
			CTraceFilter filter;
			filter.pSkip = local_player;
			game::UTIL_ClipTraceToPlayers(vecSrc, vecEnd + vecDir * 40.0f, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);
		}

		if (tr.fraction == 1.0f)
			break;

		/************* MATERIAL DETECTION ***********/
		surfacedata_t *pSurfaceData = g_physprops->GetSurfaceData(tr.surface.surfaceProps);
		int iEnterMaterial = pSurfaceData->game.material;

		GetMaterialParameters(iEnterMaterial, flPenetrationModifier, flDamageModifier);

		bool hitGrate = tr.contents & CONTENTS_GRATE;

		// since some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE, we'll trust the
		// CONTENTS_GRATE and use a high damage modifier.
		if (hitGrate)
		{
			// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
			flPenetrationModifier = 1.0f;
			flDamageModifier = 0.99f;
		}

		//calculate the damage based on the distance the bullet travelled.
		flCurrentDistance += tr.fraction * flDistance;
		fCurrentDamage *= pow(flRangeModifier, (flCurrentDistance / 500));

		auto player = reinterpret_cast<C_CSPlayer*>(tr.m_pEnt);
		if (player == target_player && tr.hitgroup != HITGROUP_GENERIC && tr.hitgroup != HITGROUP_GEAR)
		{
			return scale_damage(fCurrentDamage, tr.hitgroup, player->get_armor_value(),
				player->get_has_helmet(), weapon_data->m_flArmorRatio) > config.aimbot_mindmg;
		}

		// check if we reach penetration distance, no more penetrations after that
		if (flCurrentDistance > flPenetrationDistance && iPenetration > 0)
			iPenetration = 0;

		// check if bullet can penetarte another entity
		if (iPenetration == 0 && !hitGrate)
			break; // no, stop

		// If we hit a grate with iPenetration == 0, stop on the next thing we hit
		if (iPenetration < 0)
			break;

		Vector penetrationEnd;

		// try to penetrate object, maximum penetration is 128 inch
		if (!TraceToExit(tr.endpos, vecDir, penetrationEnd, 24, 128))
			break;

		// find exact penetration exit
		trace_t exitTr;
		game::UTIL_TraceLine(penetrationEnd, tr.endpos, CS_MASK_SHOOT | CONTENTS_HITBOX, NULL, &exitTr);

		if (exitTr.m_pEnt != tr.m_pEnt && exitTr.m_pEnt != NULL)
		{
			// something was blocking, trace again
			game::UTIL_TraceLine(penetrationEnd, tr.endpos, CS_MASK_SHOOT | CONTENTS_HITBOX, exitTr.m_pEnt, 0, &exitTr);
		}

		// get material at exit point
		pSurfaceData = g_physprops->GetSurfaceData(exitTr.surface.surfaceProps);
		int iExitMaterial = pSurfaceData->game.material;

		hitGrate = hitGrate && (exitTr.contents & CONTENTS_GRATE);

		// if enter & exit point is wood or metal we assume this is 
		// a hollow crate or barrel and give a penetration bonus
		if (iEnterMaterial == iExitMaterial)
		{
			if (iExitMaterial == CHAR_TEX_WOOD ||
				iExitMaterial == CHAR_TEX_METAL)
			{
				flPenetrationModifier *= 2;
			}
		}

		float flTraceDistance = VectorLength(exitTr.endpos - tr.endpos);

		// check if bullet has enough power to penetrate this distance for this material
		if (flTraceDistance > (flPenetrationPower * flPenetrationModifier))
			break; // bullet hasn't enough power to penetrate this distance

		// penetration was successful

		//setup new start end parameters for successive trace

		flPenetrationPower -= flTraceDistance / flPenetrationModifier;
		flCurrentDistance += flTraceDistance;

		vecSrc = exitTr.endpos;
		flDistance = (flDistance - flCurrentDistance) * 0.5;

		// reduce damage power each time we hit something other than a grate
		fCurrentDamage *= flDamageModifier;

		// reduce penetration counter
		iPenetration--;
	}

	return false;
}

void caimbot::move(CUserCmd* pCmd, bool& sendpacket)
{
	if (!config.aimbot_enabled)
		return;

	auto local_player = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (!local_player)
		return;

	auto weapon = local_player->GetActiveWeapon();
	if (!weapon)
		return;

	if (weapon->get_clip() <= 0)
		return;

	if (!config.aimbot_smooth)
	{
		const auto server_time = local_player->get_tick_base() * g_globals->interval_per_tick;
		if (server_time <= weapon->get_next_primary_attack() || server_time <= local_player->get_next_attack())
			return;
	}
	
	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return;

	auto entity = get_best_target();
	if (!entity)
		return;

	Vector current_angles;
	g_engine->GetViewAngles(current_angles);

	auto hitboxpos = entity::get_hitbox_position(entity, config.aimbot_hitbox);
	auto delta = local_player->get_eye_position() - hitboxpos;
	Vector angles;
	math::VectorAngles(delta, angles);
	angles -= local_player->get_aim_punch() * 2.f;
	math::clamp_angles(angles);

	if (config.aimbot_smooth)
	{
		delta = angles - current_angles;
		math::clamp_angles(delta);
		delta /= config.aimbot_smooth;
		math::clamp_angles(delta);
		angles = delta + current_angles;
	}

	math::clamp_angles(angles);

	const auto old_angles = pCmd->viewangles;
	const auto old_side_move = pCmd->sidemove;
	const auto old_forward_move = pCmd->forwardmove;

	if (!config.aimbot_on_key || (config.aimbot_on_key && PressedKeys[config.aimbot_key]))
	{
		if (config.aimbot_nospread)
			apply_nospread_angles(pCmd, local_player, angles);
		pCmd->viewangles = angles;
		if (!config.aimbot_silent)
			g_engine->SetViewAngles(angles);
		else if (config.aimbot_psilent)
			sendpacket = false;

		if (config.aimbot_autofire)
			pCmd->buttons |= IN_ATTACK;
	}

	math::correct_movement(old_angles, pCmd, old_forward_move, old_side_move);
}

C_CSPlayer* caimbot::get_best_target()
{
	C_CSPlayer* bestentity = nullptr;
	auto bestfov = config.aimbot_fov;

	Vector current_angles;
	g_engine->GetViewAngles(current_angles);

	auto local = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	for (auto i = 0; i < g_entitylist->GetHighestEntityIndex(); i++)
	{
		auto entity = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(i));

		if (!entity
			|| entity->IsDormant()
			|| entity->get_health() <= 0
			|| entity->get_life_state() == 1
			|| entity->get_team_num() == local->get_team_num()
			|| entity == local)
			continue;

		auto clientclass = entity->GetClientClass();
		if (!clientclass)
			continue;

		if (clientclass->m_ClassID != CCSPlayer)
			continue;

		auto delta = local->get_eye_position() - entity::get_hitbox_position(entity, config.aimbot_hitbox);
		Vector angles;
		math::VectorAngles(delta, angles);
		math::clamp_angles(angles);

		if (!can_penetrate_point(local, entity, angles))
			continue;

		delta = angles - current_angles;
		delta.y = std::remainderf(delta.y, 360.f);
		auto fov = delta.Length2D();

		if (fov < bestfov)
		{
			bestfov = fov;
			bestentity = entity;
		}
	}

	return bestentity;
}
#pragma once

#include "../interfaces.h"
#include "../sdk/CBaseEntity.h"

namespace entity
{
	Vector get_bone_position(C_CSPlayer * pEntity, int bone);
	Vector get_hitbox_position(C_CSPlayer * pEntity, int HitboxID);
	void fix_trace_ray(Vector end, Vector start, trace_t * oldtrace, C_BasePlayer * ent);
	bool is_visible(C_CSPlayer* pEntity, int HitboxID);
	bool SetupBones(matrix3x4 * pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
}
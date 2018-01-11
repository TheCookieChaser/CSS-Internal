#pragma once

#include "../interfaces.h"
#include "../sdk/CBaseEntity.h"

namespace Entity
{
	Vector GetBonePosition(C_CSPlayer * pEntity, int bone);
	Vector GetHitboxPosition(C_CSPlayer * pEntity, int HitboxID);
	void FixTraceRay(Vector end, Vector start, trace_t * oldtrace, C_BasePlayer * ent);
	bool IsVisible(C_CSPlayer* pEntity, int HitboxID);
}
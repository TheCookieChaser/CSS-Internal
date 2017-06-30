#pragma once

#include "SDK\SDK.h"
#include "SDK\CBaseEntity.h"

namespace Entity
{
	Vector GetBonePosition(C_CSPlayer * pEntity, int bone);
	Vector GetHitboxPosition(C_CSPlayer * pEntity, int HitboxID);
	bool IsVisible(C_CSPlayer* pEntity, int HitboxID);
}
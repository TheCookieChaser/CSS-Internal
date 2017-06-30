#pragma once

#include "Hooks.h"
#include "Entity.h"
#include "Math.h"
#include "Settings.h"

namespace Aimbot
{
	C_CSPlayer* GetBestTarget(CUserCmd * pCmd);
	void CreateMove(CUserCmd* pCmd);
}
#pragma once

#include "SDK\CBaseEntity.h"
#include "DrawManager.h"
#include "Settings.h"
#include "Entity.h"

namespace Visuals
{
	void EndScene();
	D3DCOLOR GetPlayerColor(C_CSPlayer * pEntity);
	D3DCOLOR GetHealthColor(int health);
	void DrawESP();
}
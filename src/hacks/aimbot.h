#pragma once

#include "../hooks/hooks.h"
#include "../tools/entity.h"
#include "../tools/math.h"
#include "../tools/config.h"

class caimbot
{
public:
	void move(CUserCmd* pCmd);
	C_CSPlayer* get_best_target();
};

extern caimbot* aimbot;
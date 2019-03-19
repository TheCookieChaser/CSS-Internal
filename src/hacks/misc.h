#pragma once

#include "../tools/entity.h"
#include "../tools/config.h"

class cmisc
{
public:
	void move(CUserCmd* cmd);
	void autojump(CUserCmd * cmd);
	void engine_prediction(C_BasePlayer * player, CUserCmd * ucmd);
};

extern cmisc* misc;
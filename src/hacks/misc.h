#pragma once

#include "../tools/entity.h"
#include "../tools/config.h"

class cmisc
{
public:
	void move(CUserCmd* cmd);
	void autojump(CUserCmd * cmd);
};

extern cmisc* misc;
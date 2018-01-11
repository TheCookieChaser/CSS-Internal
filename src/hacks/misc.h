#pragma once

#include "../tools/entity.h"
#include "../tools/config.h"

class cmisc
{
public:
	auto move(CUserCmd* pCmd) -> void;
};

extern cmisc* misc;
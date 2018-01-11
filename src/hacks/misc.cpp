#include "Misc.h"

cmisc* misc = new cmisc();

auto cmisc::move(CUserCmd* pCmd) -> void
{
	auto local = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (config.misc_autojump)
	{
		if (GetAsyncKeyState(VK_SPACE) && !(local->get_flags() & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;
	}
}
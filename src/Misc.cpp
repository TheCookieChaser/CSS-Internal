#include "Misc.h"

void Misc::CreateMove(CUserCmd* pCmd) 
{
	auto pLocal = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	if (Settings::Misc::BunnyHop)
	{
		if (GetAsyncKeyState(VK_SPACE) && !(pLocal->get_flags() & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;
	}
}
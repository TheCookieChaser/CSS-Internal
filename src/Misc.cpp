#include "Misc.h"

void Misc::CreateMove(CUserCmd* pCmd) 
{
	auto pLocal = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));
	if (Settings::Misc::BunnyHop)
	{
		if (GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;
	}
}
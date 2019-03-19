#include "hooks.h"
#include "../hacks/visuals.h"

void __fastcall frame_stage_notify(void* thisptr, void*, ClientFrameStage_t curStage)
{
	if (curStage == FRAME_RENDER_START)
	{
		//drawmanager->clear();
		//visuals->render();

		auto local_player = reinterpret_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
		if (local_player)
			local_player->get_flash_max_alpha() = config.visuals_no_flash ? 0.f : 255.f;
	}

	o_frame_stage_notify(thisptr, curStage);
}
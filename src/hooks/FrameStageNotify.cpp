#include "hooks.h"
#include "../hacks/visuals.h"

void __fastcall frame_stage_notify(void* thisptr, void*, ClientFrameStage_t curStage)
{
	if (curStage == FRAME_RENDER_START)
	{
		//drawmanager->clear();
		//visuals->render();
	}

	o_frame_stage_notify(thisptr, curStage);
}
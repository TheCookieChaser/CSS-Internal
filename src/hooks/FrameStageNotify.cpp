#include "hooks.h"
#include "../hacks/visuals.h"

void __fastcall FrameStageNotify(void* thisptr, void*, ClientFrameStage_t curStage)
{
	if (curStage == FRAME_RENDER_START)
	{
		drawmanager->clear();
		visuals->render();
	}

	oFrameStageNotify(thisptr, curStage);
}
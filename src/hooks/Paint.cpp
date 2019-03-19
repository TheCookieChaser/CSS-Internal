#include "hooks.h"
#include "../hacks/visuals.h"

void __fastcall paint(void* thisptr, void*, PaintMode_t mode)
{
	o_paint(thisptr, mode);

	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(
		tools::get_rel32(tools::find_pattern("vguimatsurface.dll", "E8 ? ? ? ? 8B 06 8D 4D D8"), 1, 5));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(
		tools::get_rel32(tools::find_pattern("vguimatsurface.dll", "E8 ? ? ? ? C7 45 ? ? ? ? ? 85 F6"), 1, 5));

	if (mode & PAINT_UIPANELS)
	{
		StartDrawing(g_surface);

		if (!visuals->is_initialized())
			visuals->initialize();

		visuals->render();

		FinishDrawing(g_surface);
	}
}
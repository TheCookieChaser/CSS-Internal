#include "hooks.h"

cvmt_hook* d3ddevice_table = nullptr;
cvmt_hook* client_table = nullptr;
cvmt_hook* input_table = nullptr;

Reset_fn			o_reset = nullptr;
EndScene_fn			o_end_scene = nullptr;
CreateMove_fn		o_create_move = nullptr;
FrameStageNotify_fn	o_frame_stage_notify = nullptr;
GetUserCmd_fn		o_get_user_cmd = nullptr;

void initialize_hooks()
{
	hWindow = FindWindowA("Valve001", nullptr);

	pOldWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWindow, GWLP_WNDPROC, LONG_PTR(wnd_proc_hooked)));

	d3ddevice_table = new cvmt_hook(g_d3ddevice);
	o_reset = d3ddevice_table->hook<Reset_fn>(16, reinterpret_cast<void*>(reset_hooked));
	o_end_scene = d3ddevice_table->hook<EndScene_fn>(42, reinterpret_cast<void*>(end_scene_hooked));

	client_table = new cvmt_hook(g_client);
	o_create_move = client_table->hook<CreateMove_fn>(21, reinterpret_cast<void*>(create_move));
	//client_table->hook(23, WriteUsercmdDeltaToBuffer);
	o_frame_stage_notify = client_table->hook<FrameStageNotify_fn>(35, reinterpret_cast<void*>(frame_stage_notify));

	input_table = new cvmt_hook(g_input);
	o_get_user_cmd = input_table->hook<GetUserCmd_fn>(8, reinterpret_cast<void*>(get_user_cmd));
}

void uninitialize_hooks()
{
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, LONG_PTR(pOldWindowProc));

	delete d3ddevice_table;
	delete client_table;
	delete input_table;
}
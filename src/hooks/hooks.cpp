#include "hooks.h"

cvmt_hook* d3ddevice_table = nullptr;
cvmt_hook* client_table = nullptr;
cvmt_hook* input_table = nullptr;

Reset_fn			oReset = nullptr;
EndScene_fn			oEndScene = nullptr;
CreateMove_fn		oCreateMove = nullptr;
FrameStageNotify_fn	oFrameStageNotify = nullptr;
GetUserCmd_fn		oGetUserCmd = nullptr;

void initialize_hooks()
{
	hWindow = FindWindowA("Valve001", nullptr);

	pOldWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc_Hooked));

	d3ddevice_table = new cvmt_hook(g_d3ddevice);
	oReset = d3ddevice_table->hook<Reset_fn>(16, Reset_Hooked);
	oEndScene = d3ddevice_table->hook<EndScene_fn>(42, EndScene_Hooked);

	client_table = new cvmt_hook(g_client);
	oCreateMove = client_table->hook<CreateMove_fn>(21, CreateMove);
	//client_table->hook(23, WriteUsercmdDeltaToBuffer);
	oFrameStageNotify = client_table->hook<FrameStageNotify_fn>(35, FrameStageNotify);

	input_table = new cvmt_hook(g_input);
	oGetUserCmd = input_table->hook<GetUserCmd_fn>(8, GetUserCmd);
}

void uninitialize_hooks()
{
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)pOldWindowProc);

	delete d3ddevice_table;
	delete client_table;
	delete input_table;
}
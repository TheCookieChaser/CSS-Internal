#include "Hooks.h"
#include "Tools.h"
#include "Interfaces.h"
#include "Settings.h"
#include "NetVarManager.h"

cvmt_hook* d3ddevice_table = nullptr;
cvmt_hook* client_table = nullptr;

Reset_fn		oReset = nullptr;
EndScene_fn		oEndScene = nullptr;
CreateMove_fn	oCreateMove = nullptr;

void Initialize_Hooks()
{
	while (!(hWindow = FindWindowA("Valve001", NULL))) 
		Sleep(200);

	pOldWindowProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc_Hooked);

	d3ddevice_table = new cvmt_hook(g_d3ddevice);
	oReset = d3ddevice_table->hook<Reset_fn>(16, Reset_Hooked);
	d3ddevice_table->hook<EndScene_fn>(42, EndScene_Hooked);

	client_table = new cvmt_hook(g_client);
	oCreateMove = client_table->hook<CreateMove_fn>(21, CreateMove_Hooked);
	client_table->hook(23, WriteUsercmdDeltaToBuffer_Hooked);
}

void Restore_Hooks()
{
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)pOldWindowProc);

	delete d3ddevice_table;
	delete client_table;
}

HINSTANCE dll;
DWORD WINAPI CheatThread(LPVOID lpvReserved)
{
	AllocConsole();
	freopen("CON", "w", stdout);
	SetConsoleTitle("css");

	initialize_interfaces();
	Initialize_Hooks();

	g_cvar->ConsoleColorPrintf(Color(150, 255, 150), "Injected Sucessfully!\n");

	while (!(GetAsyncKeyState(VK_END) & 0x8000))
	{
		if (Settings::Visuals::FarESP)
		{
			static DWORD dwDrawOnRadar = *(DWORD*)(Tools::FindSignature("client.dll", "A1 ? ? ? ? 85 C0 74 06 05") + 1);
			if (dwDrawOnRadar)
			{
				for (int i = 0; i < 32; i++)
				{
					BYTE* pDrawOnRadar = (BYTE*)((*(DWORD*)dwDrawOnRadar) + 0xD8D + i);
					*pDrawOnRadar = 1;
				}
			}
		}
	}

	g_cvar->ConsoleColorPrintf(Color(150, 255, 150), "Unloading...\n");
	FreeConsole();
	Restore_Hooks();
	Sleep(1000);
	g_cvar->ConsoleColorPrintf(Color(150, 255, 150), "Unloaded Sucessfully!\n");
	FreeLibraryAndExitThread(dll, NULL);
}

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		dll = hinstDLL;
		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(NULL, NULL, CheatThread, lpvReserved, NULL, NULL);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
		Restore_Hooks();

	return TRUE;
}
#include "Hooks.h"
#include "Tools.h"
#include "Interfaces.h"
#include "Settings.h"
#include "NetVarManager.h"

std::unique_ptr<VFTableHook> D3DDeviceTable = nullptr;
std::unique_ptr<VFTableHook> ClientTable = nullptr;

void Initialize_Hooks()
{
	D3DDeviceTable = std::make_unique<VFTableHook>((PPDWORD)d3ddevice, true);
	ClientTable = std::make_unique<VFTableHook>((PPDWORD)client, true);

	while (!(hWindow = FindWindowA("Valve001", NULL))) 
		Sleep(200);

	if (hWindow)
		pOldWindowProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc_Hooked);

	D3DDeviceTable->Hook(82, DrawIndexedPrimitive_Hooked);
	D3DDeviceTable->Hook(16, Reset_Hooked);
	D3DDeviceTable->Hook(42, EndScene_Hooked);
	ClientTable->Hook(21, CreateMove_Hooked);
	//ClientTable->Hook(23, WriteUsercmdDeltaToBuffer_Hooked); //i recreated this and it works perfectly, maybe usefull for someone
}

void Restore_Hooks()
{
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)pOldWindowProc);

	D3DDeviceTable->RestoreTable();
	ClientTable->RestoreTable();
}

HINSTANCE dll;
DWORD WINAPI CheatThread(LPVOID lpvReserved)
{
	AllocConsole();
	freopen("CON", "w", stdout);
	SetConsoleTitle("css");

	GetInterfaces();
	NetVarManager->Initialize();
	//NetVarManager->DumpNetvars();
	Initialize_Hooks();

	while (!(GetAsyncKeyState(VK_END) & 0x8000))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}

	FreeConsole();
	Restore_Hooks();
	Sleep(1000);
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
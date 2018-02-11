#include "hooks/hooks.h"

DWORD WINAPI thread(LPVOID dll)
{
	AllocConsole();
	freopen("CON", "w", stdout);
	SetConsoleTitle("css");

	initialize_interfaces();
	initialize_hooks();

	g_cvar->ConsoleColorPrintf(Color(150, 255, 150), "Sucessfully Injected!\n");

	while (!(GetAsyncKeyState(VK_END) & 0x8000))
	{
		Sleep(100);
	}

	FreeConsole();
	uninitialize_hooks();
	Sleep(1000);
	g_cvar->ConsoleColorPrintf(Color(255, 50, 50), "Sucessfully Unloaded!\n");
	FreeLibraryAndExitThread(static_cast<HMODULE>(dll), EXIT_SUCCESS);
}

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (hinstDLL)
			DisableThreadLibraryCalls(hinstDLL);
		CreateThread(nullptr, 0, thread, hinstDLL, 0, nullptr);
	}

	return TRUE;
}
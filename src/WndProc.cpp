#include "Hooks.h"
#include "Interfaces.h"
#include "Settings.h"

bool PressedKeys[256]{ false };
bool bWasInitialized = false;
WNDPROC pOldWindowProc = nullptr;
HWND hWindow = nullptr;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT STDMETHODCALLTYPE WndProc_Hooked(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_KEYDOWN:
		PressedKeys[wParam] = true;
		break;
	case WM_KEYUP:
		PressedKeys[wParam] = false;
		break;
	default: break;
	}

	{
		static bool isDown = false;
		static bool isClicked = false;
		if (PressedKeys[VK_INSERT])
		{
			isClicked = false;
			isDown = true;
		}
		else if (!PressedKeys[VK_INSERT] && isDown)
		{
			isClicked = true;
			isDown = false;
		}
		else
		{
			isClicked = false;
			isDown = false;
		}

		if (isClicked) 
		{
			Settings::Menu::Opened = !Settings::Menu::Opened;

			*(int*)((DWORD)GetModuleHandleA("vguimatsurface.dll") + 0x139CC0) = !Settings::Menu::Opened;
		}
	}

	if (bWasInitialized && Settings::Menu::Opened && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(pOldWindowProc, hWnd, uMsg, wParam, lParam);
}
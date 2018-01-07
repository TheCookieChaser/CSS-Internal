#pragma once

#include <Windows.h>
#include <memory>
#include <iostream>
#include "vmthooks.h"
#include "Interfaces.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "ImGUI/imgui.h"
#include "ImGUI/DX9/imgui_impl_dx9.h"

extern cvmt_hook* d3ddevice_table;
extern cvmt_hook* client_table;

typedef HRESULT(STDMETHODCALLTYPE* Reset_fn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(STDMETHODCALLTYPE* EndScene_fn)(IDirect3DDevice9*);
typedef void(STDMETHODCALLTYPE* CreateMove_fn)(int, float, bool);

extern Reset_fn				oReset;
extern EndScene_fn			oEndScene;
extern CreateMove_fn		oCreateMove;

HRESULT STDMETHODCALLTYPE	Reset_Hooked(IDirect3DDevice9* vDevice, D3DPRESENT_PARAMETERS* Params);
HRESULT STDMETHODCALLTYPE	EndScene_Hooked(IDirect3DDevice9* vDevice);
LRESULT STDMETHODCALLTYPE	WndProc_Hooked(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void __stdcall				CreateMove_Hooked(int sequence_number, float input_sample_frametime, bool active);
bool __fastcall				WriteUsercmdDeltaToBuffer_Hooked(void* ecx, void* edx, void* buf, int from, int to, bool isnewcommand);

extern bool PressedKeys[256];
extern bool bWasInitialized;
extern WNDPROC pOldWindowProc;
extern HWND hWindow;
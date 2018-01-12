#pragma once

#include <Windows.h>
#include <memory>
#include <iostream>
#include "../tools/vmt_hook.h"
#include "../interfaces.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <imgui/imgui.h>
#include <imgui/examples/directx9_example/imgui_impl_dx9.h>

extern cvmt_hook* d3ddevice_table;
extern cvmt_hook* client_table;
extern cvmt_hook* input_table;

typedef HRESULT(STDMETHODCALLTYPE* Reset_fn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(STDMETHODCALLTYPE* EndScene_fn)(IDirect3DDevice9*);
typedef void(__stdcall* CreateMove_fn)(int, float, bool);
typedef void(__thiscall* FrameStageNotify_fn)(void*, ClientFrameStage_t);
typedef CUserCmd*(__thiscall* GetUserCmd_fn)(void*, int sequence_number);

extern Reset_fn				oReset;
extern EndScene_fn			oEndScene;
extern CreateMove_fn		oCreateMove;
extern FrameStageNotify_fn	oFrameStageNotify;
extern GetUserCmd_fn		oGetUserCmd;

HRESULT STDMETHODCALLTYPE	Reset_Hooked(IDirect3DDevice9* vDevice, D3DPRESENT_PARAMETERS* Params);
HRESULT STDMETHODCALLTYPE	EndScene_Hooked(IDirect3DDevice9* vDevice);
LRESULT STDMETHODCALLTYPE	WndProc_Hooked(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void __stdcall				CreateMove(int sequence_number, float input_sample_frametime, bool active);
bool __fastcall				WriteUsercmdDeltaToBuffer(void* ecx, void* edx, void* buf, int from, int to, bool isnewcommand);
void __fastcall				FrameStageNotify(void* thisptr, void*, ClientFrameStage_t curStage);
CUserCmd* __fastcall		GetUserCmd(void* thisptr, void*, int sequence_number);

extern bool PressedKeys[256];
extern bool bWasInitialized;
extern WNDPROC pOldWindowProc;
extern HWND hWindow;

extern void initialize_hooks();
extern void uninitialize_hooks();
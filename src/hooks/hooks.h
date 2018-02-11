#pragma once

#include <Windows.h>
#include "../tools/vmt_hook.h"
#include "../Interfaces.h"

#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <imgui/imgui.h>

extern cvmt_hook* d3ddevice_table;
extern cvmt_hook* client_table;
extern cvmt_hook* input_table;

typedef HRESULT(STDMETHODCALLTYPE* Reset_fn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(STDMETHODCALLTYPE* EndScene_fn)(IDirect3DDevice9*);
typedef void(__stdcall* CreateMove_fn)(int, float, bool);
typedef void(__thiscall* FrameStageNotify_fn)(void*, ClientFrameStage_t);
typedef CUserCmd*(__thiscall* GetUserCmd_fn)(void*, int sequence_number);

extern Reset_fn				o_reset;
extern EndScene_fn			o_end_scene;
extern CreateMove_fn		o_create_move;
extern FrameStageNotify_fn	o_frame_stage_notify;
extern GetUserCmd_fn		o_get_user_cmd;

HRESULT STDMETHODCALLTYPE	reset_hooked(IDirect3DDevice9* vDevice, D3DPRESENT_PARAMETERS* Params);
HRESULT STDMETHODCALLTYPE	end_scene_hooked(IDirect3DDevice9* vDevice);
LRESULT STDMETHODCALLTYPE	wnd_proc_hooked(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void __stdcall				create_move(int sequence_number, float input_sample_frametime, bool active);
bool __fastcall				write_usercmd_delta_to_buffer(void* ecx, void* edx, void* buf, int from, int to, bool isnewcommand);
void __fastcall				frame_stage_notify(void* thisptr, void*, ClientFrameStage_t curStage);
CUserCmd* __fastcall		get_user_cmd(void* thisptr, void*, int sequence_number);

extern bool PressedKeys[256];
extern bool was_initialized;
extern WNDPROC pOldWindowProc;
extern HWND hWindow;

extern void initialize_hooks();
extern void uninitialize_hooks();
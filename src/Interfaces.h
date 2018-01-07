#pragma once

#include "SDK\SDK.h"
#include "Tools.h"
#include <d3d9.h>
#include <d3dx9.h>

//------------------------------------------//
// CreateInterface
//------------------------------------------//
extern IBaseClientDll*		g_client;
extern IClientEntityList*	g_entitylist;
extern IVEngineClient*		g_engine;
extern IVModelInfo*			g_modelinfo;
extern IEngineTrace*		g_trace;
extern ICvar*				g_cvar;
//------------------------------------------//
// Pattern Scan
//------------------------------------------//
extern CInput*				g_input;
extern CBaseClientState*	g_clientstate;
extern IDirect3DDevice9*	g_d3ddevice;
//------------------------------------------//
// Initialize Interfaces
//------------------------------------------//
extern void initialize_interfaces();
#pragma once

#include "sdk/sdk.h"
#include <d3d9.h>
#include <d3dx9.h>

//------------------------------------------//
// CreateInterface
//------------------------------------------//
extern IBaseClientDll*		g_client;
extern IClientEntityList*	g_entitylist;
extern IGameMovement*		g_gamemovement;
extern IPrediction*			g_prediction;
extern IVEngineClient*		g_engine;
extern IVModelInfo*			g_modelinfo;
extern IEngineTrace*		g_trace;
extern IVDebugOverlay*		g_debugoverlay;
extern ICvar*				g_cvar;
//------------------------------------------//
// Pattern Scan
//------------------------------------------//
extern CInput*				g_input;
extern CBaseClientState*	g_clientstate;
extern IDirect3DDevice9*	g_d3ddevice;
extern CGlobalVars*			g_globalvars;
extern IMoveHelper*			g_movehelper;
//------------------------------------------//
// Initialize Interfaces
//------------------------------------------//
extern void initialize_interfaces();
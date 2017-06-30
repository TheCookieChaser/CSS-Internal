#pragma once

#include "SDK\SDK.h"
#include "Tools.h"

extern DWORD d3ddevice;
extern ICvar* cvar;
extern IBaseClientDll* client;
extern CInput* input;
extern IVEngineClient* engine;
extern IClientEntityList* entitylist;
extern IVModelInfo* modelinfo;
extern CBaseClientState* clientstate;
extern IEngineTrace* trace;

void GetInterfaces();
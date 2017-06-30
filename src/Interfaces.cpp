#include "Interfaces.h"

DWORD d3ddevice;
ICvar* cvar;
IBaseClientDll* client;
CInput* input;
IVEngineClient* engine;
IClientEntityList* entitylist;
IVModelInfo* modelinfo;
CBaseClientState* clientstate;
IEngineTrace* trace;

void GetInterfaces()
{
	CreateInterfaceFn vstdlibFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "CreateInterface"));
	CreateInterfaceFn clientFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface"));
	CreateInterfaceFn engineFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface"));

	d3ddevice = **reinterpret_cast<DWORD**>(Tools::FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 8D 53 08") + 0x1);
	cvar = reinterpret_cast<ICvar*>(vstdlibFactory("VEngineCvar004", nullptr));
	client = reinterpret_cast<IBaseClientDll*>(clientFactory("VClient017", nullptr));
	input = **reinterpret_cast<CInput***>(Tools::FindSignature("client.dll", "8B 0D ? ? ? ? 8B 01 FF 60 44") + 0x2);
	engine = reinterpret_cast<IVEngineClient*>(engineFactory("VEngineClient014", nullptr));
	entitylist = reinterpret_cast<IClientEntityList*>(clientFactory("VClientEntityList003", nullptr));
	modelinfo = reinterpret_cast<IVModelInfo*>(engineFactory("VModelInfoClient006", nullptr));
	clientstate = *reinterpret_cast<CBaseClientState**>(Tools::FindSignature("engine.dll", "83 3D ? ? ? ? ? B8 ? ? ? ? B9 ? ? ? ? 0F 4D C1") + 0x2);
	trace = reinterpret_cast<IEngineTrace*>(engineFactory("EngineTraceClient003", nullptr));
}
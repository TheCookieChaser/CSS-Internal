#include "Interfaces.h"

//------------------------------------------//
// CreateInterface
//------------------------------------------//
IBaseClientDll*		g_client = nullptr;
IClientEntityList*	g_entitylist = nullptr;
IVEngineClient*		g_engine = nullptr;
IVModelInfo*		g_modelinfo = nullptr;
IEngineTrace*		g_trace = nullptr;
ICvar*				g_cvar = nullptr;
//------------------------------------------//
// Pattern Scan
//------------------------------------------//
CInput*				g_input = nullptr;
CBaseClientState*	g_clientstate = nullptr;
IDirect3DDevice9*	g_d3ddevice = nullptr;
//------------------------------------------//
// Initialize Interfaces
//------------------------------------------//
void initialize_interfaces()
{
	//------------------------------------------//
	// Factories
	//------------------------------------------//
	auto client_factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface"));
	auto engine_factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface"));
	auto vstdlib_factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "CreateInterface"));
	//------------------------------------------//
	// CreateInterface
	//------------------------------------------//
	g_client = reinterpret_cast<IBaseClientDll*>(client_factory("VClient017", nullptr));
	g_entitylist = reinterpret_cast<IClientEntityList*>(client_factory("VClientEntityList003", nullptr));
	g_engine = reinterpret_cast<IVEngineClient*>(engine_factory("VEngineClient014", nullptr));
	g_modelinfo = reinterpret_cast<IVModelInfo*>(engine_factory("VModelInfoClient006", nullptr));
	g_trace = reinterpret_cast<IEngineTrace*>(engine_factory("EngineTraceClient003", nullptr));
	g_cvar = reinterpret_cast<ICvar*>(vstdlib_factory("VEngineCvar004", nullptr));
	//------------------------------------------//
	// Pattern Scan
	//------------------------------------------//
	g_input = **reinterpret_cast<CInput***>(Tools::FindSignature("client.dll", "8B 0D ? ? ? ? 8B 01 FF 60 44") + 0x2);
	g_clientstate = *reinterpret_cast<CBaseClientState**>(Tools::FindSignature("engine.dll", "83 3D ? ? ? ? ? B8 ? ? ? ? B9 ? ? ? ? 0F 4D C1") + 0x2);
	g_d3ddevice = **reinterpret_cast<IDirect3DDevice9***>(Tools::FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 8D 53 08") + 0x1);
}
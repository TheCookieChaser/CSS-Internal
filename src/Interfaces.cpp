#include "interfaces.h"
#include "tools/tools.h"

//------------------------------------------//
// CreateInterface
//------------------------------------------//
IBaseClientDll*		g_client = nullptr;
IClientEntityList*	g_entitylist = nullptr;
IGameMovement*		g_gamemovement = nullptr;
IPrediction*		g_prediction = nullptr;
IVEngineClient*		g_engine = nullptr;
IVModelInfo*		g_modelinfo = nullptr;
IEngineTrace*		g_trace = nullptr;
IVDebugOverlay*		g_debugoverlay = nullptr;
ICvar*				g_cvar = nullptr;
IPhysicsSurfaceProps* g_physprops = nullptr;
CEngineVGui*		g_enginevgui = nullptr;
ISurface*			g_surface = nullptr;
//------------------------------------------//
// Pattern Scan
//------------------------------------------//
CInput*				g_input = nullptr;
CBaseClientState*	g_clientstate = nullptr;
IDirect3DDevice9*	g_d3ddevice = nullptr;
CGlobalVars*		g_globals = nullptr;
IMoveHelper*		g_movehelper = nullptr;
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
	auto vphysics_factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vphysics.dll"), "CreateInterface"));
	auto vguimatsurface_factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vguimatsurface.dll"), "CreateInterface"));
	//------------------------------------------//
	// CreateInterface
	//------------------------------------------//
	g_client = reinterpret_cast<IBaseClientDll*>(client_factory("VClient017", nullptr));
	g_entitylist = reinterpret_cast<IClientEntityList*>(client_factory("VClientEntityList003", nullptr));
	g_gamemovement = reinterpret_cast<IGameMovement*>(client_factory("GameMovement001", nullptr));
	g_prediction = reinterpret_cast<IPrediction*>(client_factory("VClientPrediction001", nullptr));
	g_engine = reinterpret_cast<IVEngineClient*>(engine_factory("VEngineClient014", nullptr));
	g_modelinfo = reinterpret_cast<IVModelInfo*>(engine_factory("VModelInfoClient006", nullptr));
	g_trace = reinterpret_cast<IEngineTrace*>(engine_factory("EngineTraceClient003", nullptr));
	g_debugoverlay = reinterpret_cast<IVDebugOverlay*>(engine_factory("VDebugOverlay003", nullptr));
	g_cvar = reinterpret_cast<ICvar*>(vstdlib_factory("VEngineCvar004", nullptr));
	g_physprops = reinterpret_cast<IPhysicsSurfaceProps*>(vphysics_factory("VPhysicsSurfaceProps001", nullptr));
	g_enginevgui = reinterpret_cast<CEngineVGui*>(engine_factory("VEngineVGui001", nullptr));
	g_surface = reinterpret_cast<ISurface*>(vguimatsurface_factory("VGUI_Surface030", nullptr));
	//------------------------------------------//
	// Pattern Scan
	//------------------------------------------//
	g_input = **reinterpret_cast<CInput***>(
		tools::find_pattern("client.dll", "8B 0D ? ? ? ? 8B 01 FF 60 44") + 2);
	g_clientstate = *reinterpret_cast<CBaseClientState**>(
		tools::find_pattern("engine.dll", "83 3D ? ? ? ? ? B8 ? ? ? ? B9 ? ? ? ? 0F 4D C1") + 2);
	g_d3ddevice = **reinterpret_cast<IDirect3DDevice9***>(
		tools::find_pattern("shaderapidx9.dll", "A1 ? ? ? ? 8D 53 08") + 1);
	g_globals = **reinterpret_cast<CGlobalVars***>((
		*reinterpret_cast<uintptr_t**>(g_client))[0] + 0x34);
	g_movehelper = **reinterpret_cast<IMoveHelper***>(
		tools::find_pattern("client.dll", "FF 35 ? ? ? ? 50 8B 11 56") + 2);
}
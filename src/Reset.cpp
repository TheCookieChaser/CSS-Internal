#include "Hooks.h"
#include "DrawManager.h"

HRESULT STDMETHODCALLTYPE Reset_Hooked(IDirect3DDevice9* vDevice, D3DPRESENT_PARAMETERS* Params)
{
	if (!bWasInitialized)
		return D3DDeviceTable->GetOriginal<Reset_t>(16)(vDevice, Params);

	ImGui_ImplDX9_InvalidateDeviceObjects();
	DrawManager->InvalidateObjects();

	auto ret = D3DDeviceTable->GetOriginal<Reset_t>(16)(vDevice, Params);

	DrawManager->CreateObjects();
	ImGui_ImplDX9_CreateDeviceObjects();

	return ret;
}
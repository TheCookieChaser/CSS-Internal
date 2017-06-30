#include "Hooks.h"

HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive_Hooked(IDirect3DDevice9* vDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	return D3DDeviceTable->GetOriginal<DrawIndexedPrimitive_t>(82)(vDevice, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
}
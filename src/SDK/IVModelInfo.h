#pragma once

class IVModelInfo
{
public:
	studiohdr_t* GetStudiomodel(const void* model)
	{
		typedef studiohdr_t*(__thiscall* GetStudiomodel_t)(void*, const void*);
		return CallVFunction<GetStudiomodel_t>(this, 28)(this, model);
	}
};
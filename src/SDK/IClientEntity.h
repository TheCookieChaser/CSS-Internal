#pragma once

#include "SDK.h"
#include "../tools/virtuals.h"

struct bf_read;
using CBaseHandle = unsigned long;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {};
};

class IClientUnknown : public IHandleEntity
{
public:
	virtual ~IClientUnknown() {};
};

class IClientThinkable
{
public:
	virtual ~IClientThinkable() {};
};

class IClientRenderable
{
public:
	virtual ~IClientRenderable() {};

	const void*	GetModel()
	{
		typedef const void*(__thiscall* GetModel_t)(void*);
		return get_vfunc<GetModel_t>(this, 9)(this);
	}

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* SetupBones_t)(void*, matrix3x4*, int, int, float);
		return get_vfunc<SetupBones_t>(this, 16)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	void GetRenderBounds(Vector& mins, Vector& maxs)
	{
		typedef bool(__thiscall* SetupBones_t)(void*, Vector&, Vector&);
		get_vfunc<SetupBones_t>(this, 20)(this, mins, maxs);
	}
};

struct ClientClass;
class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;
	virtual void			NotifyShouldTransmit(int state) = 0;
	virtual void			OnPreDataChanged(int updateType) = 0;
	virtual void			OnDataChanged(int updateType) = 0;
	virtual void			PreDataUpdate(int updateType) = 0;
	virtual void			PostDataUpdate(int updateType) = 0;
	virtual bool			IsDormant() = 0;
	virtual int				GetIndex() const = 0;
	virtual void			ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities() = 0;
};

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual ~IClientEntity() {};
};
#pragma once

#include "sdk.h"

#define	MASK_ALL					(0xFFFFFFFF)

struct Ray_t
{
	__declspec(align(16)) Vector  m_Start;
	__declspec(align(16)) Vector  m_Delta;
	__declspec(align(16)) Vector  m_StartOffset;
	__declspec(align(16)) Vector  m_Extents;
	//without your matrix3x4
	bool	m_IsRay;
	bool	m_IsSwept;

	void Init(Vector const& start, Vector const& end)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents.x = m_Extents.y = m_Extents.z = 0.0f;

		m_IsRay = true;

		m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;

		m_Start = start;
	}
};

class CBaseTrace
{
public:
	Vector                  startpos;
	Vector                  endpos;
	cplane_t                plane;
	float                   fraction;
	int                             contents;
	unsigned short  dispFlags;
	bool                    allsolid;
	bool                    startsolid;
};

struct csurface_t
{
	const char*             name;
	short                   surfaceProps;
	unsigned short  flags;
};

class CGameTrace : public CBaseTrace
{
public:

	// Returns true if hEnt points at the world entity.
	// If this returns true, then you can't use GetHitBoxIndex().
	bool DidHitWorld() const;

	// Returns true if we hit something and it wasn't the world.
	bool DidHitNonWorldEntity() const;

	// Gets the entity's network index if the trace has hit an entity.
	// If not, returns -1.
	int GetEntityIndex() const;

	// Returns true if there was any kind of impact at all
	bool DidHit() const;
public:

	float		fractionleftsolid;		// time we left a solid, only valid if we started in solid
	csurface_t	surface;				// surface hit (impact surface)

	int			hitgroup;				// 0 == generic, non-zero is specific body part
	short		physicsbone;			// physics bone hit by trace in studio

#if defined( CLIENT_DLL )
	C_BaseEntity *m_pEnt;
#else
	IClientEntity *m_pEnt;
#endif

	// NOTE: this member is overloaded.
	// If hEnt points at the world entity, then this is the static prop index.
	// Otherwise, this is the hitbox index.
	int			hitbox;					// box hit by trace in studio

	CGameTrace() {}

private:
	// No copy constructors allowed
	CGameTrace(const CGameTrace& vOther);
};


//-----------------------------------------------------------------------------
// Returns true if there was any kind of impact at all
//-----------------------------------------------------------------------------
inline bool CGameTrace::DidHit() const
{
	return fraction < 1 || allsolid || startsolid;
}


typedef CGameTrace trace_t;

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
	virtual bool            ShouldHitEntity(IClientEntity* pEntity, int contentsMask) = 0;
	virtual TraceType_t     GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
	CTraceFilterSkipTwoEntities(void *pPassEnt1, void *pPassEnt2)
	{
		pSkip1 = pPassEnt1;
		pSkip2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(IClientEntity *pEntityHandle, int contentsMask)
	{
		return  !(pEntityHandle == pSkip1 || pEntityHandle == pSkip2);
	}
	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip1;
	void* pSkip2;
};

class IEngineTrace
{
public:
	int	GetPointContents(const Vector &vecAbsPosition, int contentsMask = MASK_ALL, IClientEntity** ppEntity = NULL)
	{
		typedef int(__thiscall* fnGetPointContents)(void*, const Vector&, int, IClientEntity**);
		return get_vfunc<fnGetPointContents>(this, 0)(this, vecAbsPosition, contentsMask, ppEntity);
	}

	void ClipRayToEntity(const Ray_t &ray, unsigned int fMask, IHandleEntity *pEnt, trace_t *pTrace)
	{
		typedef void(__thiscall* ClipRayToEntity_fn)(void*, const Ray_t &ray, unsigned int fMask, IHandleEntity *pEnt, trace_t *pTrace);
		get_vfunc<ClipRayToEntity_fn>(this, 2)(this, ray, fMask, pEnt, pTrace);
	}

	void TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)
	{
		typedef void(__thiscall* fnTraceRay)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		get_vfunc<fnTraceRay>(this, 4)(this, ray, fMask, pTraceFilter, pTrace);
	}
};
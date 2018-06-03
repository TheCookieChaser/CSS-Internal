#include "Entity.h"
#include "Math.h"

Vector entity::get_bone_position(C_CSPlayer* pEntity, int bone)
{
	matrix3x4_t boneMatrix[128];
	if (pEntity->SetupBones(boneMatrix, 128, 0x00000100, 0))
	{
		return Vector(boneMatrix[bone][0][3], boneMatrix[bone][1][3], boneMatrix[bone][2][3]);
	}
	return Vector(0, 0, 0);
}

Vector entity::get_hitbox_position(C_CSPlayer* pEntity, int HitboxID)
{
	matrix3x4_t matrix[128];
	if (!pEntity->SetupBones(matrix, 128, 0x00000100, 0))
		return Vector(0, 0, 0);

	auto model = pEntity->GetModel();

	if (!model)
		return Vector(0, 0, 0);

	studiohdr_t* hdr = g_modelinfo->GetStudiomodel(model);
	if (!hdr)
		return Vector(0, 0, 0);

	mstudiohitboxset_t* set = hdr->pHitboxSet(pEntity->get_hitbox_set());
	if (!set)
		return Vector(0, 0, 0);

	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox)
		return Vector(0, 0, 0);

	Vector vMin, vMax, vCenter, sCenter;
	math::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
	math::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) * 0.5f;

	return vCenter;
}

void entity::fix_trace_ray(Vector end, Vector start, trace_t* oldtrace, C_BasePlayer* ent)
{
	Vector mins, maxs;
	ent->GetRenderBounds(mins, maxs);

	auto CenterOfBBOX = (maxs + mins) * 0.5f;
	auto origin = ent->get_origin();

	Vector CenterOfEntityInWorld = (CenterOfBBOX + origin);

	Vector delta(end - start);
	delta.Normalize();

	auto v20 = (CenterOfEntityInWorld - start);
	auto ForwardDot = delta.Dot(v20);

	float v23;

	if (ForwardDot >= 0.0f)
	{
		//Player is at or in front
		if (ForwardDot <= delta.Length())
		{
			auto fuck(CenterOfEntityInWorld - ((delta * ForwardDot) + start));
			v23 = fuck.Length();
		}
		else
		{
			auto lol(CenterOfEntityInWorld - end);
			v23 = -lol.Length();
		}
	}

	else
		v23 = -v20.Length();

	if (v23 <= 60.f)
	{
		Ray_t ray;
		ray.Init(start, end);

		trace_t trace;
		g_trace->ClipRayToEntity(ray, 0x4600400B, ent, &trace);

		if (oldtrace->fraction > trace.fraction)
			*oldtrace = trace;
	}
}

bool entity::is_visible(C_CSPlayer* pEntity, int HitboxID)
{
	auto pLocal = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(g_engine->GetLocalPlayer()));
	trace_t tr;
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip = pLocal;

	auto start = pLocal->get_eye_position();
	auto end = get_hitbox_position(pEntity, HitboxID);
	ray.Init(start,end);
	g_trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	//FixTraceRay(end, start, &tr, pEntity);

	return (tr.m_pEnt == pEntity || tr.fraction >= 0.99f);
}

bool entity::SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	
}
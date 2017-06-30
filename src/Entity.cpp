#include "Entity.h"
#include "Math.h"

Vector Entity::GetBonePosition(C_CSPlayer* pEntity, int bone)
{
	matrix3x4 boneMatrix[128];
	if (pEntity->SetupBones(boneMatrix, 128, 0x00000100, 0))
	{
		return Vector(boneMatrix[bone][0][3], boneMatrix[bone][1][3], boneMatrix[bone][2][3]);
	}
	return Vector(0, 0, 0);
}

Vector Entity::GetHitboxPosition(C_CSPlayer* pEntity, int HitboxID)
{
	matrix3x4 matrix[128];
	if (!pEntity->SetupBones(matrix, 128, 0x00000100, 0))
		return Vector(0, 0, 0);

	auto model = pEntity->GetModel();

	if (!model)
		return Vector(0, 0, 0);

	studiohdr_t* hdr = modelinfo->GetStudiomodel(model);
	if (!hdr)
		return Vector(0, 0, 0);

	mstudiohitboxset_t* set = hdr->pHitboxSet(pEntity->GetHitboxSet());
	if (!set)
		return Vector(0, 0, 0);

	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox)
		return Vector(0, 0, 0);

	Vector vMin, vMax, vCenter, sCenter;
	Math::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
	Math::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) * 0.5f;

	return vCenter;
}

bool Entity::IsVisible(C_CSPlayer* pEntity, int HitboxID)
{
	auto pLocal = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));
	trace_t tr;
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip = pLocal;

	ray.Init(pLocal->GetEyePosition(), GetHitboxPosition(pEntity, HitboxID));
	trace->TraceRay(ray, MASK_NPCWORLDSTATIC | CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX, &filter, &tr);

	return (tr.m_pEnt == pEntity || tr.fraction == 1.0f);
}
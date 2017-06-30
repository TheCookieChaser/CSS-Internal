#pragma once

#include "SDK/SDK.h"
#include <iostream>

namespace Math
{
	void inline SinCos(float a, float* s, float*c);
	void AngleVectors(const Vector &angles, Vector &forward);
	float DotProduct(const Vector & a, const Vector & b);
	void VectorTransform(const Vector in1, float in2[3][4], Vector & out);
	void NormalizeAngles(Vector& angle);
	void ClampAngles(Vector& angle);
	void CorrectMovement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	float GetFov(const Vector &viewAngle, const Vector &aimAngle);
	void VectorAngles(const Vector &forward, Vector &angles);
	Vector CalcAngle(Vector src, Vector dst);
}
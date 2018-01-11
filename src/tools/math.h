#pragma once

#include "../sdk/sdk.h"
#include <iostream>
#include <algorithm>

namespace math
{
	void SinCos(float a, float* s, float* c);
	void AngleVectors(const Vector &angles, Vector &forward);
	void AngleVectors(const Vector & angles, Vector * forward, Vector * right, Vector * up);
	float DotProduct(const Vector & a, const Vector & b);
	void VectorTransform(const Vector in1, float in2[3][4], Vector & out);
	void VectorAngles(const Vector &forward, Vector &angles);

	void clamp_angles(Vector& angle);
	void correct_movement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
}
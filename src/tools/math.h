#pragma once

#include "../SDK/sdk.h"
#include <iostream>
#include <algorithm>

namespace math
{
	void MatrixSetColumn(const Vector & in, int column, matrix3x4_t & out);
	void SinCos(float a, float* s, float* c);
	void AngleVectors(const Vector &angles, Vector &forward);
	void AngleVectors(const Vector & angles, Vector * forward, Vector * right, Vector * up);
	__forceinline float DotProduct(const Vector & a, const Vector & b);
	__forceinline vec_t DotProduct(const vec_t * v1, const vec_t * v2);
	void VectorTransform(const Vector in1, float in2[3][4], Vector & out);
	void VectorTransform(const float * in1, const matrix3x4_t & in2, float * out);
	void VectorTransform(const Vector in1, matrix3x4_t in2, Vector & out);
	void VectorAngles(const Vector &forward, Vector &angles);

	void AngleMatrix(const QAngle & angles, matrix3x4_t & matrix);

	void AngleMatrix(const QAngle & angles, const Vector & position, matrix3x4_t & matrix);

	void clamp_angles(Vector& angle);
	void correct_movement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
}
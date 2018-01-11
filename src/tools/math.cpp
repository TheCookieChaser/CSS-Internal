#include "math.h"

void math::SinCos(float a, float* s, float*c)
{
	*s = sin(a);
	*c = cos(a);
}

void math::AngleVectors(const Vector &angles, Vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float sp, sy, cp, cy;

	math::SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	math::SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

void math::AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up)
{
	float angle;
	static float sr, sp, sy, cr, cp, cy, cpi = (M_PI * 2 / 360);

	angle = angles.y * cpi;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles.x * cpi;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles.z * cpi;
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr * -sy);
		up->y = (cr*sp*sy + -sr * cy);
		up->z = cr * cp;
	}
}

FORCEINLINE float math::DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void math::VectorTransform(const Vector in1, float in2[3][4], Vector &out)
{
	out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

void math::VectorAngles(const Vector& forward, Vector &angles)
{
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
		angles[1] = 0.0f;  //yaw left/right
	}
	else
	{
		angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
		angles[1] = atan2(forward[1], forward[0]) * 180 / M_PI;

		if (angles[1] > 90)
			angles[1] -= 180;
		else if (angles[1] < 90)
			angles[1] += 180;
		else if (angles[1] == 90)
			angles[1] = 0;
	}

	angles[2] = 0.0f;
}

void math::correct_movement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/forward move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

void math::clamp_angles(Vector& angle)
{
	angle.x = std::clamp(angle.x, -89.f, 89.f);
	angle.y = std::remainderf(angle.y, 360.f);
}
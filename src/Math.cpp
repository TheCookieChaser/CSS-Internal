#include "math.h"

void Math::SinCos(float a, float* s, float*c)
{
	*s = sin(a);
	*c = cos(a);
}

void Math::AngleVectors(const Vector &angles, Vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float sp, sy, cp, cy;

	Math::SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	Math::SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

FORCEINLINE float Math::DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void Math::VectorTransform(const Vector in1, float in2[3][4], Vector &out)
{
	out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

void Math::NormalizeAngles(Vector& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

void Math::ClampAngles(Vector& angle)
{
	std::clamp<float>(angle.y, -180.0f, 180.0f);
	std::clamp<float>(angle.x, -89.0f, 89.0f);
	std::clamp<float>(angle.z, -50.0f, 50.0f);
}

void Math::CorrectMovement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
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

float Math::GetFov(const Vector& viewAngle, const Vector& aimAngle)
{
	Vector delta = aimAngle - viewAngle;
	NormalizeAngles(delta);

	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

void Math::VectorAngles(const Vector& forward, Vector &angles)
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

Vector Math::CalcAngle(Vector src, Vector dst)
{
	Vector angles;
	Vector delta = src - dst;

	Math::VectorAngles(delta, angles);

	delta.Normalize();

	return angles;
}
#include "game.h"
#include "../SDK/sdk.h"
#include "../interfaces.h"
#include "../SDK/CBaseEntity.h"
#include "../tools/math.h"

auto game::RandomFloat(float min, float max) -> float
{
	static auto oRandomFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
	return oRandomFloat(min, max);
}

auto game::RandomSeed(std::uint32_t seed) -> void
{
	static auto oRandomSeed = reinterpret_cast<void(*)(std::uint32_t)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));
	oRandomSeed(seed);
}

void game::UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter *filter, trace_t *tr)
{
	static auto function = reinterpret_cast<decltype(&UTIL_ClipTraceToPlayers)>(
		tools::find_pattern("client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 18"));
	function(vecAbsStart, vecAbsEnd, mask, filter, tr);
}

void game::UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask,
	ITraceFilter *pFilter, trace_t *ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);

	g_trace->TraceRay(ray, mask, pFilter, ptr);
}

void game::UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask,
	void *ignore, int collisionGroup, trace_t *ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter traceFilter;
	traceFilter.pSkip = ignore;

	g_trace->TraceRay(ray, mask, &traceFilter, ptr);
}
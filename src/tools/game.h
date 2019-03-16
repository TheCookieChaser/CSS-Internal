#pragma once

#include <Windows.h>
#include <cstdint>
#include "../SDK/IEngineTrace.h"

namespace game
{
	auto RandomFloat(float min, float max) -> float;
	auto RandomSeed(std::uint32_t seed) -> void;
	void UTIL_ClipTraceToPlayers(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, ITraceFilter * filter, trace_t * tr);
	void UTIL_TraceLine(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, ITraceFilter * pFilter, trace_t * ptr);
	void UTIL_TraceLine(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, void * ignore, int collisionGroup, trace_t * ptr);
}

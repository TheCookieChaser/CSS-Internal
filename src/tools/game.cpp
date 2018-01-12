#include "game.h"

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
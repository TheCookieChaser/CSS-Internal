#pragma once

#include <Windows.h>
#include <cstdint>

namespace game
{
	auto RandomFloat(float min, float max) -> float;
	auto RandomSeed(std::uint32_t seed) -> void;
}
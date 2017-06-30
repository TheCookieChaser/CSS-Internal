#pragma once

#include <Windows.h>
#include <memory>
#include <Psapi.h>
#include <chrono>
#include <thread>

namespace Tools
{
	uint64_t FindSignature(const char* szModule, const char* szSignature);
}
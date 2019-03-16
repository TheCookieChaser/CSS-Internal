#pragma once

#include <Windows.h>
#include <memory>
#include <Psapi.h>
#include <chrono>
#include <thread>

namespace tools
{
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

	uintptr_t find_pattern(const char* szModule, const char* szSignature);
	std::uintptr_t get_rel32(std::uintptr_t address, std::uintptr_t offset, std::uintptr_t instruction_size);
	bool IsCodePtr(void * ptr);
}
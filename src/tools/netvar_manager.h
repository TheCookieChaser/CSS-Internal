#pragma once
#include "../sdk/IBaseClientDll.h"
#include "fnv_hash.h"
#include <map>
#include <type_traits>

class netvar_manager
{
private:
	struct stored_data
	{
		RecvProp* prop_ptr;
		std::uint16_t class_relative_offset;
	};

public:
	// Return a const instance, others shouldnt modify this.
	static auto get() -> const netvar_manager&
	{
		static netvar_manager instance;
	return instance;
	}

		auto get_offset(const fnv::hash hash) const -> std::uint16_t
	{
		return m_props.at(hash).class_relative_offset;
	}

	auto get_prop(const fnv::hash hash) const -> RecvProp*
	{
		return m_props.at(hash).prop_ptr;
	}

	// Prevent instruction cache pollution caused by automatic
	// inlining of `get` and get_offset every netvar usage when
	// there are a lot of netvars
	__declspec(noinline) static auto get_offset_by_hash(const fnv::hash hash) -> std::uint16_t
	{
		return get().get_offset(hash);
	}

	template<fnv::hash Hash>
	static auto to_address(void* ptr) -> std::uintptr_t
	{
		static auto offset = std::uint16_t(0);
		if (!offset)
			offset = get_offset_by_hash(Hash);
		return std::uintptr_t(ptr) + offset;
	}

private:
	netvar_manager();
	auto dump_recursive(const char* base_class, RecvTable* table, std::uint16_t offset) -> void;

private:
	std::map<fnv::hash, stored_data> m_props;
};


#define PNETVAR_OFFSET(funcname, class_name, var_name, offset, ...) \
auto funcname() -> std::add_pointer_t<__VA_ARGS__> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = netvar_manager::to_address<hash>(this); \
	return reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(addr); \
}

#define PNETVAR(funcname, class_name, var_name, ...) \
	PNETVAR_OFFSET(funcname, class_name, var_name, 0, __VA_ARGS__)

#define NETVAR_OFFSET(funcname, class_name, var_name, offset, ...) \
auto funcname() -> std::add_lvalue_reference_t<__VA_ARGS__> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = netvar_manager::to_address<hash>(this); \
	return *reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(addr); \
}

#define NETVAR(funcname, class_name, var_name, ...) \
	NETVAR_OFFSET(funcname, class_name, var_name, 0, __VA_ARGS__)

#define NETPROP(funcname, class_name, var_name) \
static auto funcname() ->  RecvProp* \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	static auto prop_ptr = netvar_manager::get().get_prop(hash); \
	return prop_ptr; \
}
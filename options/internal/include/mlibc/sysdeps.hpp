#pragma once

#include <mlibc/sysdeps-interface.hpp>
#include <mlibc/os-sysdeps.hpp>

template<mlibc::RequiredSysdeps Backend>
struct SysdepsImpl : public Backend {};

extern constinit SysdepsImpl<mlibc::Sysdeps> sysdeps;

template <typename T>
struct member_class_type;

template <typename C, typename R, typename... Args>
struct member_class_type<R (C::*)(Args...)> {
	using type = C;
};

template <auto MemberFunc, typename Base>
constexpr bool hasSysdep() {
	using MemberType = decltype(MemberFunc);
	using OwnerClass = typename member_class_type<MemberType>::type;

	return !std::is_same_v<OwnerClass, Base>;
}

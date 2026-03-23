#ifndef MLIBC_ALL_SYSDEPS
#define MLIBC_ALL_SYSDEPS

#include <mlibc-config.h>
#include <internal-config.h>

#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/sysdeps.hpp>
#include <utility>

namespace mlibc {

// Concept that checks whether a sysdep is implemented;
template <typename Tag>
concept IsImplemented = !std::same_as<Sysdeps<Tag>, NoImpl>;

// Concept that returns whether the sysdep is marked [[noreturn]].
template <typename Tag>
concept IsNoReturn = requires { requires Tag::is_noreturn == true; };

template <typename Tag, typename... Args>
using sysdep_return_t = std::invoke_result_t<SysdepImpl<Tag>, Args...>;

// Wrapper for calling a sysdep.
template <typename Tag, typename... Args>
    requires(IsImplemented<Tag> && !IsNoReturn<Tag>)
inline sysdep_return_t<Tag, Args...> sysdep(Args &&...args) {
	return Sysdeps<Tag>::operator()(std::forward<Args>(args)...);
}

// Wrapper for calling a sysdep, selected when the sysdep is marked [[noreturn]].
template <typename Tag, typename... Args>
    requires(IsImplemented<Tag> && IsNoReturn<Tag>)
[[noreturn]] inline void sysdep(Args &&...args) {
	Sysdeps<Tag>::operator()(std::forward<Args>(args)...);
}

// Placeholder wrapper that is selected when a missing sysdep gets mistakenly used.
template <typename Tag, typename... Args>
    requires(!IsImplemented<Tag>)
[[gnu::error("Unimplemented sysdep called!")]]
sysdep_return_t<Tag, Args...> sysdep(Args &&...) {
	static_assert(false, "Unimplemented sysdep called!");
}

// Wrapper that calls an optional sysdep, or returns ENOSYS if it is not implemented.
template <typename Tag, typename... Args>
inline sysdep_return_t<Tag, Args...> sysdep_or_enosys(Args &&...args) {
	if constexpr (IsImplemented<Tag>) {
		return Sysdeps<Tag>::operator()(std::forward<Args>(args)...);
	} else {
		__ensure_warn("Library function fails due to missing sysdep", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		return ENOSYS;
	}
}

template<typename Tag, typename... Args>
inline sysdep_return_t<Tag, Args...> sysdep_or_panic(Args &&... args) {
	if constexpr (IsImplemented<Tag>) {
		return Sysdeps<Tag>::operator() (std::forward<Args>(args)...);
	} else {
		__ensure_warn("Library function fails due to missing sysdep", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		__builtin_trap();
	}
}

// ANCHOR: mandatory-sysdeps
// Ensure that required sysdeps are implemented.
static_assert(IsImplemented<Exit>);
static_assert(IsImplemented<FutexWait>);
static_assert(IsImplemented<FutexWake>);
static_assert(IsImplemented<Open>);
static_assert(IsImplemented<Read>);
static_assert(IsImplemented<Write>);
static_assert(IsImplemented<Seek>);
static_assert(IsImplemented<Close>);
static_assert(IsImplemented<ClockGet>);
static_assert(IsImplemented<LibcLog>);
static_assert(IsImplemented<LibcPanic>);
static_assert(IsImplemented<AnonAllocate>);
static_assert(IsImplemented<AnonFree>);
static_assert(IsImplemented<VmMap>);
static_assert(IsImplemented<VmUnmap>);
static_assert(IsImplemented<TcbSet>);
// ANCHOR_END: mandatory-sysdeps

} // namespace mlibc

#endif /* MLIBC_ALL_SYSDEPS */

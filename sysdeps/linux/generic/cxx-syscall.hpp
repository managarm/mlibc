#pragma once

#include <errno.h>
#include <mlibc/tcb.hpp>
#include <mlibc/thread.hpp>
#include <mlibc-config.h>
#include <utility>

#include <sys/syscall.h>
#include <bits/syscall.h>

using sc_word_t = __sc_word_t;

extern "C" {
	extern sc_word_t __mlibc_do_asm_cp_syscall(int sc, sc_word_t arg1, sc_word_t arg2,
			sc_word_t arg3, sc_word_t arg4, sc_word_t arg5, sc_word_t arg6);

	extern void __mlibc_do_cancel();
}

namespace mlibc {
	// C++ wrappers for the extern "C" functions.
	inline sc_word_t do_nargs_syscall(int sc) {
		return __do_syscall0(sc);
	}
	inline sc_word_t do_nargs_syscall(int sc, sc_word_t arg1) {
		return __do_syscall1(sc, arg1);
	}
	inline sc_word_t do_nargs_syscall(int sc, sc_word_t arg1, sc_word_t arg2) {
		return __do_syscall2(sc, arg1, arg2);
	}
	inline sc_word_t do_nargs_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
		return __do_syscall3(sc, arg1, arg2, arg3);
	}
	inline sc_word_t do_nargs_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4) {
		return __do_syscall4(sc, arg1, arg2, arg3, arg4);
	}
	inline sc_word_t do_nargs_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5) {
		return __do_syscall5(sc, arg1, arg2, arg3, arg4, arg5);
	}
	inline sc_word_t do_nargs_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
		return __do_syscall6(sc, arg1, arg2, arg3, arg4, arg5, arg6);
	}

	inline sc_word_t do_nargs_cp_syscall(int sc, sc_word_t arg1) {
		return __mlibc_do_asm_cp_syscall(sc, arg1, 0, 0, 0, 0, 0);
	}
	inline sc_word_t do_nargs_cp_syscall(int sc, sc_word_t arg1, sc_word_t arg2) {
		return __mlibc_do_asm_cp_syscall(sc, arg1, arg2, 0, 0, 0, 0);
	}
	inline sc_word_t do_nargs_cp_syscall(int sc, sc_word_t arg1, sc_word_t arg2,
			sc_word_t arg3) {
		return __mlibc_do_asm_cp_syscall(sc, arg1, arg2, arg3, 0, 0, 0);
	}
	inline sc_word_t do_nargs_cp_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4) {
		return __mlibc_do_asm_cp_syscall(sc, arg1, arg2, arg3, arg4, 0, 0);
	}
	inline sc_word_t do_nargs_cp_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5) {
		return __mlibc_do_asm_cp_syscall(sc, arg1, arg2, arg3, arg4, arg5, 0);
	}
	inline sc_word_t do_nargs_cp_syscall(int sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
		return __mlibc_do_asm_cp_syscall(sc, arg1, arg2, arg3, arg4, arg5, arg6);
	}

	// Type-safe syscall result type.
	enum class sc_result_t : sc_word_t { };

	// Cast to the argument type of the extern "C" functions.
	inline sc_word_t sc_cast(long x) { return x; }
	inline sc_word_t sc_cast(const void *x) { return reinterpret_cast<sc_word_t>(x); }

	template<typename... T>
	sc_result_t do_syscall(int sc, T... args) {
		return static_cast<sc_result_t>(do_nargs_syscall(sc, sc_cast(args)...));
	}

	inline int sc_error(sc_result_t ret) {
		auto v = static_cast<sc_word_t>(ret);
		if(static_cast<unsigned long>(v) > -4096UL)
			return -v;
		return 0;
	}

	template<typename... T>
	sc_result_t do_cp_syscall(int sc, T... args) {
#if __MLIBC_POSIX_OPTION && !MLIBC_BUILDING_RTLD
		auto result = static_cast<sc_result_t>(do_nargs_cp_syscall(sc, sc_cast(args)...));
		if (int e = sc_error(result); e) {
			auto tcb = reinterpret_cast<Tcb*>(get_current_tcb());
			if (tcb_cancelled(tcb->cancelBits) && e == EINTR) {
				__mlibc_do_cancel();
				__builtin_unreachable();
			}
		}
		return result;
#else
		return do_syscall(sc, std::forward<T>(args)...);
#endif // __MLIBC_POSIX_OPTION || !MLIBC_BUILDING_RTLD
	}
	// Cast from the syscall result type.
	template<typename T>
	T sc_int_result(sc_result_t ret) {
		auto v = static_cast<sc_word_t>(ret);
		return v;
	}

	template<typename T>
	T *sc_ptr_result(sc_result_t ret) {
		auto v = static_cast<sc_word_t>(ret);
		return reinterpret_cast<T *>(v);
	}
}

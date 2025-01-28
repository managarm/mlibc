
#include <bits/ensure.h>
#include <fenv.h>

// The functions that are not in this file but are defined in the header
// are implemented like musl does in assembly.
extern "C" __attribute__((__visibility__("hidden"))) int __fesetround(int);

int fegetexceptflag(fexcept_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int feholdexcept(fenv_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fesetexceptflag(const fexcept_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fesetround(int r) {
	if (r != FE_TONEAREST
#ifdef FE_DOWNWARD
		&& r != FE_DOWNWARD
#endif
#ifdef FE_UPWARD
		&& r != FE_UPWARD
#endif
#ifdef FE_TOWARDZERO
		&& r != FE_TOWARDZERO
#endif
	)
		return -1;
	return __fesetround(r);
}

int feupdateenv(const fenv_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


#include <bits/ensure.h>
#include <fenv.h>
#include <type_traits>

// The functions that are not in this file but are defined in the header
// are implemented like musl does in assembly.
extern "C" __attribute__((__visibility__("hidden"))) int __fesetround(int);

// our implementation of fegetexceptflag requires that this holds.
static_assert(std::is_integral_v<fexcept_t>);

int fegetexceptflag(fexcept_t *fe, int excepts) {
	*fe = fetestexcept(excepts);
	return 0;
}

int feholdexcept(fenv_t *fe) {
	fegetenv(fe);
	feclearexcept(FE_ALL_EXCEPT);
	return 0;
}

int fesetexceptflag(const fexcept_t *fe, int mask) {
	feclearexcept(~*fe & mask);
	feraiseexcept(*fe & mask);
	return 0;
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

int feupdateenv(const fenv_t *fe) {
	int e = fetestexcept(FE_ALL_EXCEPT);
	fesetenv(fe);
	feraiseexcept(e);
	return 0;
}

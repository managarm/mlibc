#include "libm.h"
#include <limits.h>

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
int ilogbl(long double x) { return ilogb(x); }
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
int ilogbl(long double x) {
#pragma STDC FENV_ACCESS ON
	union ldshape u = {x};
	uint64_t m = u.i.m;
	int e = u.i.se & 0x7fff;

	if (!e) {
		if (m == 0) {
			FORCE_EVAL(0 / 0.0f);
			return FP_ILOGB0;
		}
		/* subnormal x */
		for (e = -0x3fff + 1; m >> 63 == 0; e--, m <<= 1)
			;
		return e;
	}
	if (e == 0x7fff) {
		FORCE_EVAL(0 / 0.0f);
		return m << 1 ? FP_ILOGBNAN : INT_MAX;
	}
	return e - 0x3fff;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
int ilogbl(long double x) {
#pragma STDC FENV_ACCESS ON
	union ldshape u = {x};
	int e = u.i.se & 0x7fff;

	if (!e) {
		if (x == 0) {
			FORCE_EVAL(0 / 0.0f);
			return FP_ILOGB0;
		}
		/* subnormal x */
		x *= 0x1p120;
		return ilogbl(x) - 120;
	}
	if (e == 0x7fff) {
		FORCE_EVAL(0 / 0.0f);
		u.i.se = 0;
		return u.f ? FP_ILOGBNAN : INT_MAX;
	}
	return e - 0x3fff;
}
#elif LDBL_MANT_DIG == 106 && LDBL_MAX_EXP == 1024
int ilogbl(long double x) {
	union ldshape u = {x};

	uint64_t hx = u.i1.hi & 0x7fffffffffffffffLL;
	if (hx <= 0x0010000000000000LL) {
		if (hx == 0) {
			return FP_ILOGB0;
		} else {
			hx <<= 11;
			int ix = 0;
			for (ix = -1022; hx > 0; hx <<= 1)
				ix -= 1;
			return ix;
		}
	} else if (hx < 0x7ff0000000000000LL) {
		int hexp = (hx >> 52) - 0x3ff;
		if ((hx & 0x000fffffffffffffLL) == 0) {
			if ((u.i1.hi ^ u.i1.lo) < 0 && (u.i1.lo & 0x7fffffffffffffffLL) != 0)
				hexp--;
		}
		return hexp;
	} else if (FP_ILOGBNAN != INT_MAX) {
		if (hx == 0x7ff0000000000000LL)
			return INT_MAX;
	}

	return FP_ILOGBNAN;
}
#endif

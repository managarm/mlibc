#include "libm.h"
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double fabsl(long double x) { return fabs(x); }
#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384
long double fabsl(long double x) {
	union ldshape u = {x};

	u.i.se &= 0x7fff;
	return u.f;
}
#elif LDBL_MANT_DIG == 106 && LDBL_MAX_EXP == 1024
long double fabsl(long double x) {
	union ldshape u = {x};

	u.i1.lo = u.i1.lo ^ (u.i1.hi & 0x8000000000000000LL);
	u.i1.hi = u.i1.hi & 0x7fffffffffffffffLL;

	return u.f;
}
#endif

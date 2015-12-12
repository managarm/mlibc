
#include <math.h>
#include <immintrin.h>

#include <mlibc/ensure.h>

#include <stdint.h>

#pragma GCC visibility push(hidden)
#include <frigg/debug.hpp>
#pragma GCC visibility pop

namespace ieee754 {

struct SoftDouble {
	typedef uint64_t Bits;
	typedef uint64_t Mantissa;
	typedef int16_t Exp;

	static constexpr int kMantissaBits = 52;
	static constexpr int kExpBits = 11;
	static constexpr int kBias = 1023;
	
	// this exponent represents zeros (when mantissa = 0) and subnormals (when mantissa != 0)
	static constexpr Exp kSubExp = -kBias;
	// this exponent represents infinities (when mantissa = 0) and NaNs (when mantissa != 0)
	static constexpr Exp kInfExp = ((Exp(1) << kExpBits) - 1) - kBias;

	static constexpr Bits kMantissaMask = (Bits(1) << kMantissaBits) - 1;
	static constexpr Bits kExpMask = ((Bits(1) << kExpBits) - 1) << kMantissaBits;
	static constexpr Bits kSignMask = Bits(1) << (kMantissaBits + kExpBits);

	SoftDouble(bool negative, Mantissa mantissa, Exp exp)
	: negative(negative), mantissa(mantissa), exp(exp) {
//		frigg::infoLogger.log() << "(" << (int)negative << ", " << (void *)mantissa
//				<< ", " << exp << ")" << frigg::EndLog();
		__ensure(mantissa < (Mantissa(1) << kMantissaBits));
		__ensure((exp + kBias) >= 0);
		__ensure((exp + kBias) < (Exp(1) << kExpBits));
	}

	const bool negative;
	const Mantissa mantissa;
	const Exp exp;
};

template<typename F>
using Bits = typename F::Bits;

template<typename F>
using Mantissa = typename F::Mantissa;

template<typename F>
using Exp = typename F::Exp;

template<typename F>
bool isZero(F x) {
	return x.exp == F::kSubExp && x.mantissa == 0;
}

template<typename F>
bool isFinite(F x) {
	return x.exp != F::kInfExp;
}

// --------------------------------------------------------
// Soft float operations
// --------------------------------------------------------

template<typename F>
F constZero(bool negative) {
	return F(negative, 0, F::kSubExp);
}

template<typename F>
F constOne(bool negative) {
	return F(negative, 0, 0);
}

template<typename F>
F floor(F x) {
	if(!isFinite(x) || isZero(x)) // TODO: need exception for the not-finite case?
		return x;
	
	if(x.exp > F::kMantissaBits)
		return x; // x is already integral
	
	if(x.exp < 0) {
		// TODO: raise inexact
		// return -1 or +0
		return x.negative ? constOne<F>(true) : constZero<F>(false);
	}
	
	Mantissa<F> mask = F::kMantissaMask >> x.exp;
	if(!(x.mantissa & mask))
		return x; // x is already integral
	
	// TODO: raise inexact
	Mantissa<F> integral_position = (Mantissa<F>(1) << F::kMantissaBits) >> x.exp;
	if(x.negative)
		return F(true, (x.mantissa + integral_position) & (~mask), x.exp);
	return F(false, x.mantissa & (~mask), x.exp);
}

template<typename F>
F ceil(F x) {
	if(!isFinite(x) || isZero(x)) // TODO: need exception for the not-finite case?
		return x;
	
	if(x.exp > F::kMantissaBits)
		return x; // x is already integral
	
	if(x.exp < 0) {
		// TODO: raise inexact
		// return -0 or +1
		return x.negative ? constZero<F>(true) : constOne<F>(false);
	}
	
	Mantissa<F> mask = F::kMantissaMask >> x.exp;
	if(!(x.mantissa & mask))
		return x; // x is already integral
	
	// TODO: raise inexact
	Mantissa<F> integral_position = (Mantissa<F>(1) << F::kMantissaBits) >> x.exp;
	if(x.negative)
		return F(true, x.mantissa & (~mask), x.exp);
	return F(false, (x.mantissa + integral_position) & (~mask), x.exp);
}

// --------------------------------------------------------
// Soft float <-> bit string conversion functions
// --------------------------------------------------------

template<typename F>
uint64_t compileBits(F soft) {
	auto bits = Bits<F>(soft.mantissa) | ((Bits<F>(soft.exp) + F::kBias) << soft.kMantissaBits);
	return soft.negative ? (F::kSignMask | bits) : bits;
}

SoftDouble extractBits(uint64_t bits) {
	return SoftDouble(bits & SoftDouble::kSignMask, bits & SoftDouble::kMantissaMask,
			((bits & SoftDouble::kExpMask) >> SoftDouble::kMantissaBits) - SoftDouble::kBias);
}

// --------------------------------------------------------
// Soft float -> native float conversion functions
// --------------------------------------------------------

union DoubleBits {
	double fp;
	uint64_t bits;
};

double compileNative(SoftDouble soft) {
	DoubleBits word;
	word.bits = compileBits(soft);
	return word.fp;
}

SoftDouble extractNative(double native) {
	DoubleBits word;
	word.fp = native;
	return extractBits(word.bits);
}

} // namespace ieee754

int __mlibc_fpclassify(double x) {
	return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
}
int __mlibc_fpclassifyf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __mlibc_fpclassifyl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double acos(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float acosf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double acosl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double asin(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float asinf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double asinl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double atan(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float atanf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double atanl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double atan2(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float atan2f(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double atan2l(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double cos(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float cosf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double cosl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double sin(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float sinf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double sinl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double tan(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float tanf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double tanl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double acosh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float acoshf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double acoshl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double asinh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float asinhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double asinhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double atanh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float atanhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double atanhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double cosh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float coshf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double coshl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double sinh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float sinhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double sinhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double tanh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float tanhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double tanhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double exp(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float expf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double expl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double exp2(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float exp2f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double exp2l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double expm1(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float expm1f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double expm1l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double frexp(double x, int *power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float frexpf(float x, int *power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double frexpl(long double x, int *power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double ilogb(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float ilogbf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double ilogbl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double ldexp(double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float ldexpf(float x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double ldexpl(long double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float logf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double logl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log10(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float log10f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double log10l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log1p(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float log1pf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double log1pl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log2(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float log2f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double log2l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double logb(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float logbf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double logbl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double modf(double x, double *integral) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float modff(float x, float *integral) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double modfl(long double x, long double *integral) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double scalbn(double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float scalbnf(float x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double scalbnl(long double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double scalbln(double x, long power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float scalblnf(float x, long power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double scalblnl(long double x, long power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double cbrt(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float cbrtf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double cbrtl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fabs(double x) {
	return signbit(x) ? -x : x;
}
float fabsf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fabsl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double hypot(double x, double y) {
	__ensure(isfinite(x));
	__ensure(isfinite(y));
	// TODO: fix exception handling
	double u = fabs(x);
	double v = fabs(y);
	if(u > v)
		return u * sqrt(1 + (v / u) * (v / u));
	return v * sqrt(1 + (u / v) * (u / v));
}
float hypotf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double hypotl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double pow(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float powf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double powl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double sqrt(double x) {
	auto sse_x = _mm_set_sd(x);
	return _mm_cvtsd_f64(_mm_sqrt_sd(sse_x, sse_x));
}
float sqrtf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double sqrtl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double erf(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float erff(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double erfl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double erfc(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float erfcf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double erfcl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double lgamma(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float lgammaf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double lgammal(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double tgamma(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float tgammaf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double tgammal(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double ceil(double x) {
	auto soft_x = ieee754::extractNative(x);
	auto result = ieee754::ceil(soft_x);
	return ieee754::compileNative(result);
}
float ceilf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double ceill(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double floor(double x) {
	auto soft_x = ieee754::extractNative(x);
	auto result = ieee754::floor(soft_x);
	return ieee754::compileNative(result);
}
float floorf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double floorl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nearbyint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nearbyintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nearbyintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double rint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float rintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double rintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long lrint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lrintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lrintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long long llrint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llrintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llrintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double round(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float roundf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double roundl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long lround(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lroundf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lroundl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long long llround(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llroundf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llroundl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double trunc(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float truncf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double truncl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fmod(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float fmodf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fmodl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double remainder(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float remainderf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double remainderl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double remquo(double x, double y, int *quotient) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float remquof(float x, float y, int *quotient) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double remquol(long double x, long double y, int *quotient) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double copysign(double x, double sign) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float copysignf(float x, float sign) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double copysignl(long double x, long double sign) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nan(const char *tag) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nanf(const char *tag) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nanl(const char *tag) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nextafter(double x, double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nextafterf(float x, float dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nextafterl(long double x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nexttoward(double x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nexttowardf(float x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nexttowardl(long double x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fdim(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float fdimf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fdiml(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fmax(double x, double y) {
	__ensure(isfinite(x) && isfinite(y));
	return x < y ? y : x;
}
float fmaxf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fmaxl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fmin(double x, double y) {
	__ensure(isfinite(x) && isfinite(y));
	return x < y ? x : y;
}
float fminf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fminl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

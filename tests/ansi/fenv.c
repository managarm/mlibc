#include <assert.h>
#include <stdbool.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

/* qemu-m68k < version 9.1.0 cannot run the test due to missing FPU logic. */
#ifdef __m68k__
int main() {
	return 0;
}
#else
#define NO_OPTIMIZE(x) asm volatile("" :: "r,m" (x) : "memory")

static void div_by_zero() {
	volatile float zero = 0.0f;
	NO_OPTIMIZE(69.0f / zero);
}

static bool float_cmp(float a, float b) {
	return a == b || fabs(a - b) < (fabs(a) + fabs(b)) * FLT_EPSILON;
}

static void test_rounding(float expectation1, float expectation2) {
	float x;
	volatile float f = 1.968750f;
	volatile float m = 0x1.0p23f;

	NO_OPTIMIZE(x = f + m);
	assert(float_cmp(expectation1, x));
	NO_OPTIMIZE(x = x - m);
	assert(x == expectation2);
}

void test0() {
	// test whether the divide-by-zero exception is raised
	feclearexcept(FE_ALL_EXCEPT);
	assert(fetestexcept(FE_ALL_EXCEPT) == 0);

	div_by_zero();
	int raised = fetestexcept(FE_DIVBYZERO);
	assert((raised & FE_DIVBYZERO));
}

void test1() {
	// test various rounding modes
	feclearexcept(FE_DIVBYZERO);
	assert(fetestexcept(FE_ALL_EXCEPT) == 0);

	fesetround(FE_UPWARD);
	assert(fegetround() == FE_UPWARD);
	test_rounding(8388610.0f, 2.0f);

	fesetround(FE_DOWNWARD);
	assert(fegetround() == FE_DOWNWARD);
	test_rounding(8388609.0f, 1.0f);

	fesetround(FE_TONEAREST);
	assert(fegetround() == FE_TONEAREST);
	test_rounding(8388610.0f, 2.0f);

	fesetround(FE_TOWARDZERO);
	assert(fegetround() == FE_TOWARDZERO);
	test_rounding(8388609.0f, 1.0f);
}

void test2() {
	// test feraiseexcept
	feclearexcept(FE_ALL_EXCEPT);
	assert(fetestexcept(FE_ALL_EXCEPT) == 0);
	assert(feraiseexcept(FE_DIVBYZERO | FE_OVERFLOW) == 0);
	assert(fetestexcept(FE_ALL_EXCEPT) == (FE_DIVBYZERO | FE_OVERFLOW));
}

void test3() {
	// test fe{get,set}env
	feclearexcept(FE_ALL_EXCEPT);
	assert(fetestexcept(FE_ALL_EXCEPT) == 0);
	assert(feraiseexcept(FE_OVERFLOW) == 0);

	fenv_t state;
	assert(fegetenv(&state) == 0);
	assert(fetestexcept(FE_ALL_EXCEPT) == FE_OVERFLOW);

	div_by_zero();
	assert(fetestexcept(FE_ALL_EXCEPT) == (FE_DIVBYZERO | FE_OVERFLOW));

	assert(fesetenv(&state) == 0);
	assert(fetestexcept(FE_ALL_EXCEPT) == FE_OVERFLOW);
}

int main() {
	test0();
	test1();
	test2();
	test3();

	return 0;
}
#endif

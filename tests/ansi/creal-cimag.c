#include <assert.h>
#include <math.h>
#include <complex.h>

// FIXME: We should create a proper floating point facility
//        in order for other functions to be tested properly

#define APPROXIMATELY_EQUAL(calculated, expected) fabs((calculated) - (expected)) <= 0.0000005
#define APPROXIMATELY_EQUALF(calculated, expected) fabsf((calculated) - (expected)) <= 0.0000005f
#define APPROXIMATELY_EQUALL(calculated, expected) fabsl((calculated) - (expected)) <= 0.0000005L

#define IS_COMPLEX_NUMBER(Z)    \
    _Generic((Z),               \
        double complex: 1,      \
        float complex: 1,       \
        long double complex: 1, \
        default: 0              \
    )

int main() {
#if !(defined(USE_HOST_LIBC) && defined(__clang__))
    assert(IS_COMPLEX_NUMBER(CMPLX(5.2, 4.3)));
    double complex cz = CMPLX(5.2, 4.3);
    assert(APPROXIMATELY_EQUAL(creal(cz), 5.2));
    assert(APPROXIMATELY_EQUAL(cimag(cz), 4.3));

    assert(IS_COMPLEX_NUMBER(CMPLXF(1.2f, 2.5f)));
    float complex czf = CMPLXF(1.2f, 2.5f);
    assert(APPROXIMATELY_EQUALF(crealf(czf), 1.2f));
    assert(APPROXIMATELY_EQUALF(cimagf(czf), 2.5f));

    assert(IS_COMPLEX_NUMBER(CMPLXL(0.1L, 123.54L)));
    long double complex czl = CMPLXL(0.1L, 123.54L);
    assert(APPROXIMATELY_EQUALL(creall(czl), 0.1L));
    assert(APPROXIMATELY_EQUALL(cimagl(czl), 123.54L));
#endif

    return 0;
}

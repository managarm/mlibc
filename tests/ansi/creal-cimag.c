#include <assert.h>
#include <complex.h>

// FIXME: We should create a proper floating point facility
//        in order for other functions to be tested properly
// For these we can rely on operator == since we don't make 
// calculations

#define IS_COMPLEX_NUMBER(Z)    \
    _Generic((Z),               \
        double complex: 1,      \
        float complex: 1,       \
        long double complex: 1, \
        default: 0              \
    )

int main() {
    assert(IS_COMPLEX_NUMBER(CMPLX(5.2, 4.3)));
    double complex cz = CMPLX(5.2, 4.3);
    assert(creal(cz) == 5.2);
    assert(cimag(cz) == 4.3);

    assert(IS_COMPLEX_NUMBER(CMPLXF(1.2f, 2.5f)));
    float complex czf = CMPLXF(1.2f, 2.5f);
    assert(creal(czf) == 1.2f);
    assert(cimag(czf) == 2.5f);

    assert(IS_COMPLEX_NUMBER(CMPLXL(0.1L, 123.54L)));
    long double complex czl = CMPLXL(0.1L, 123.54L);
    assert(creal(czl) == 0.1L);
    assert(cimag(czl) == 123.54L);

    return 0;
}
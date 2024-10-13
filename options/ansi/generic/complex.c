#include <complex.h>

long double cimagl(long double complex z) {
    return __imag__(z);
}

long double creall(long double complex z) {
    return __real__(z);
}

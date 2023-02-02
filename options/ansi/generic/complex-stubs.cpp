#include <complex.h>

extern "C" {

double cimag(double complex z) {
    return __imag__(z);
}

float cimagf(float complex z) {
    return __imag__(z);
}

long double cimagl(long double complex z) {
    return __imag__(z);
}

double creal(double complex z) {
    return __real__(z);
}

float crealf(float complex z) {
    return __real__(z);
}

long double creall(long double complex z) {
    return __real__(z);
}


}

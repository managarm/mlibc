#include <complex.h>
#include <math.h>

long double cabsl(long double complex z) {
	return hypotl(creall(z), cimagl(z));
}

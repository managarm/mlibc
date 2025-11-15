#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <math.h>

extern int __signgam;
double __lgamma_r(double, int *);

double lgamma(double x)
{
	return __lgamma_r(x, &__signgam);
}

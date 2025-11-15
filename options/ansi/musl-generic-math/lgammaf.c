#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <math.h>

extern int __signgam;
float __lgammaf_r(float, int *);

float lgammaf(float x)
{
	return __lgammaf_r(x, &__signgam);
}

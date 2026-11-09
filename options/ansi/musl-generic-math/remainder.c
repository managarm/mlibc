#include <math.h>
#include "helpers.h"

double remainder(double x, double y)
{
	int q;
	return remquo(x, y, &q);
}

weak_alias(remainder, drem);

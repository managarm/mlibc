#include <math.h>
#include "weak_alias.h"
//#include "libc.h"

int __signgam = 0;

weak_alias(__signgam, signgam);
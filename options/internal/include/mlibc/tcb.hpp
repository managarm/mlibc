#pragma once

#include <stdint.h>

struct Tcb {
	Tcb *selfPointer;
	size_t dtvSize;
	void **dtvPointers;
	int didExit;
	void *returnValue;
};


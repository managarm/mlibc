#pragma once

#include <stdint.h>
#include <bits/size_t.h>

struct Tcb {
	Tcb *selfPointer;
	size_t dtvSize;
	void **dtvPointers;
	int tid;
	int didExit;
	void *returnValue;
};


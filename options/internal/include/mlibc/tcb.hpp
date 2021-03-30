#pragma once

#include <stdint.h>
#include <bits/size_t.h>

// Gcc expects the stack canary to be at fs:0x28,
// at least on x86_64, so this struct has fixed
// ABI until stackCanary.

// The code in options/linker/aarch64/runtime.S depends on the layout
// of this struct (or at least on the position of dtvPointers)
struct Tcb {
	Tcb *selfPointer;
	size_t dtvSize;
	void **dtvPointers;
	int tid;
	int didExit;
	void *returnValue;
	uintptr_t stackCanary;
};


#pragma once

#include <stdint.h>
#include <bits/size_t.h>

/*
 * Explanation of cancellation bits:
 *
 * tcbCancelEnableBit and tcbCancelAsyncBit should be self-explanatory,
 * they are set if cancellation is enabled, or asynchronous, respectively.
 *
 * tcbCancelTriggerBit is set whenever a cancellation is triggered, which is
 * in pthread_cancel() or in the signal handler. This bit is used by
 * pthread_testcancel() to check whether a cancellation has been requested,
 * and also by cancellable syscalls.
 */

namespace {
	// Set when the cancellation is enabled
	constexpr unsigned int tcbCancelEnableBit = 1 << 0;
	// 1 - cancellation is asynchronous, 0 - cancellation is deferred
	constexpr unsigned int tcbCancelAsyncBit = 1 << 1;
	// Set when the thread has been cancelled
	constexpr unsigned int tcbCancelTriggerBit = 1 << 2;
}

namespace mlibc {
	// Returns true when bitmask indicates thread has been asynchronously
	// cancelled.
	static constexpr bool tcb_async_cancelled(int value) {
		return (value & (tcbCancelEnableBit | tcbCancelAsyncBit
				| tcbCancelTriggerBit)) == (tcbCancelEnableBit
					| tcbCancelAsyncBit | tcbCancelTriggerBit);
	}
}

// Gcc expects the stack canary to be at fs:0x28,
// at least on x86_64, so this struct has fixed
// ABI until stackCanary.
struct Tcb {
	Tcb *selfPointer;
	size_t dtvSize;
	void **dtvPointers;
	int tid;
	int didExit;
	void *returnValue;
	uintptr_t stackCanary;
	int cancelBits;
};


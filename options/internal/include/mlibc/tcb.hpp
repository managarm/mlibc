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
 *
 * tcbCancelingBit is set when a cancellation is currently being handled. This
 * is to avoid a situation in which a cancellation handler gets interrupted by
 * a SIGCANCEL and a second cancellation handler gets executed on top of the
 * previous one. Right now this cannot happen, since we stay in signal handler
 * context when canceling/exiting. In the future this might be done outside
 * of a signal handler, in which case we shouldn't restart the cancellation process.
 *
 * tcbExitingBit is set when the thread starts the exit procedure. Currently
 * this is just an exit, but in the future this will be a stack unwinding
 * procedure, which shouldn't be reentered. Not currently set anywhere,
 * may be done so in the future.
 *
 * TODO(geert): update this comment when we do unwinding in the exit procedure.
 */

namespace {
	// Set when the cancellation is enabled
	constexpr unsigned int tcbCancelEnableBit = 1 << 0;
	// 1 - cancellation is asynchronous, 0 - cancellation is deferred
	constexpr unsigned int tcbCancelAsyncBit = 1 << 1;
	// Set when the thread has been cancelled
	constexpr unsigned int tcbCancelTriggerBit = 1 << 2;
	// Set when the thread is in the process of being cancelled.
	constexpr unsigned int tcbCancelingBit = 1 << 3;
	// Set when the thread is exiting.
	constexpr unsigned int tcbExitingBit = 1 << 4;
}

namespace mlibc {
	// Returns true when bitmask indicates thread has been asynchronously
	// cancelled.
	static constexpr bool tcb_async_cancelled(int value) {
		return (value & (tcbCancelEnableBit | tcbCancelAsyncBit
				| tcbCancelTriggerBit)) == (tcbCancelEnableBit
					| tcbCancelAsyncBit | tcbCancelTriggerBit);
	}

	// Returns true when bitmask indicates async cancellation is enabled.
	static constexpr bool tcb_async_cancel(int value) {
		return (value & (tcbCancelEnableBit | tcbCancelAsyncBit))
			== (tcbCancelEnableBit | tcbCancelAsyncBit);
	}

	// Returns true when bitmask indicates cancellation is enabled.
	static constexpr bool tcb_cancel_enabled(int value) {
		return (value & tcbCancelEnableBit);
	}

	// Returns true when bitmask indicates threas has been cancelled.
	static constexpr bool tcb_cancelled(int value) {
		return (value & (tcbCancelEnableBit | tcbCancelTriggerBit))
		       == (tcbCancelEnableBit | tcbCancelTriggerBit);
	}
}

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
	int cancelBits;
	struct AtforkHandler {
		void (*prepare)(void);
		void (*parent)(void);
		void (*child)(void);

		AtforkHandler *next;
		AtforkHandler *prev;
	};

	AtforkHandler *atforkBegin;
	AtforkHandler *atforkEnd;

	struct CleanupHandler {
		void (*func)(void *);
		void *arg;

		CleanupHandler *next;
		CleanupHandler *prev;
	};

	CleanupHandler *cleanupBegin;
	CleanupHandler *cleanupEnd;
	int isJoinable;
};


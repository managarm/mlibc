
#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

// FIXME: required for hel.h
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/initializer.hpp>

#include <hel.h>
#include <hel-syscalls.h>

struct Queue {
	Queue()
	: _handle{kHelNullHandle}, _queue(nullptr), _progress(0) { }

	void recreateQueue() {
		if(!_queue)
			return;
		HEL_CHECK(helCreateQueue(_queue, 0, &_handle));
	}

	HelHandle getQueue() {
		if(!_queue) {
			auto ptr = getAllocator().allocate(sizeof(HelQueue) + 4096);
			_queue = reinterpret_cast<HelQueue *>(ptr);
			_queue->elementLimit = 128;
			_queue->queueLength = 4096;
			_queue->kernelState = 0;
			_queue->userState = 0;
			HEL_CHECK(helCreateQueue(_queue, 0, &_handle));
		}
		return _handle;
	}

	void *dequeueSingle() {
		__ensure(_queue);

		auto e = __atomic_load_n(&_queue->kernelState, __ATOMIC_ACQUIRE);
		while(true) {
			__ensure(!(e & kHelQueueWantNext));

			if(_progress != (e & kHelQueueTail)) {
				__ensure(_progress < (e & kHelQueueTail));

				auto ptr = (char *)_queue + sizeof(HelQueue) + _progress;
				auto elem = reinterpret_cast<HelElement *>(ptr);
				_progress += sizeof(HelElement) + elem->length;
				return ptr + sizeof(HelElement);
			}

			if(!(e & kHelQueueWaiters)) {
				auto d = e | kHelQueueWaiters;
				if(__atomic_compare_exchange_n(&_queue->kernelState,
						&e, d, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE))
					e = d;
			}else{
				HEL_CHECK(helFutexWait((int *)&_queue->kernelState, e));
				e = __atomic_load_n(&_queue->kernelState, __ATOMIC_ACQUIRE);
			}
		}
	}

	void trim() {
		if(!_queue)
			return;

		// for now we just reset the queue.
		_queue->kernelState = 0;
		_queue->userState = 0;
		_progress = 0;
	}

private:
	HelHandle _handle;
	HelQueue *_queue;
	size_t _progress;
};

inline HelSimpleResult *parseSimple(void *&element) {
	auto result = reinterpret_cast<HelSimpleResult *>(element);
	element = (char *)element + sizeof(HelSimpleResult);
	return result;
}

inline HelInlineResult *parseInline(void *&element) {
	auto result = reinterpret_cast<HelInlineResult *>(element);
	element = (char *)element + sizeof(HelInlineResult)
			+ ((result->length + 7) & ~size_t(7));
	return result;
}

inline HelLengthResult *parseLength(void *&element) {
	auto result = reinterpret_cast<HelLengthResult *>(element);
	element = (char *)element + sizeof(HelLengthResult);
	return result;
}

inline HelHandleResult *parseHandle(void *&element) {
	auto result = reinterpret_cast<HelHandleResult *>(element);
	element = (char *)element + sizeof(HelHandleResult);
	return result;
}

HelHandle *cacheFileTable();
void clearCachedInfos();

extern thread_local Queue globalQueue;

#endif // MLIBC_POSIX_PIPE


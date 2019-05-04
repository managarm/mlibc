
#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

// FIXME: required for hel.h
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/initializer.hpp>

#include <hel.h>
#include <hel-syscalls.h>

struct SignalGuard {
	SignalGuard();

	SignalGuard(const SignalGuard &) = delete;

	~SignalGuard();

	SignalGuard &operator= (const SignalGuard &) = delete;

private:
	sigset_t _restoreMask;
};

// We need an allocator for message structs in sysdeps functions; the "normal" mlibc
// allocator cannot be used, as the sysdeps function might be called from a signal.
MemoryAllocator &getSysdepsAllocator();

struct Queue;

struct ElementHandle {
	ElementHandle(Queue *queue, int n, void *data)
	: _queue{queue}, _n{n}, _data{data} { }

	ElementHandle(const ElementHandle &) = delete;
	
	ElementHandle &operator= (const ElementHandle &) = delete;

	~ElementHandle();

	void *data() {
		return _data;
	}

	void advance(size_t size) {
		_data = reinterpret_cast<char *>(_data) + size;
	}

private:
	Queue *_queue;
	int _n;
	void *_data;
};

struct Queue {
	Queue()
	: _handle{kHelNullHandle} {
		// We do not need to protect those allocations against signals as this constructor
		// is only called during library initialization.
		_queue = reinterpret_cast<HelQueue *>(getSysdepsAllocator().allocate(sizeof(HelQueue)
				+ 2 * sizeof(int)));
		_chunks[0] = reinterpret_cast<HelChunk *>(getSysdepsAllocator().allocate(sizeof(HelChunk) + 4096));
		_chunks[1] = reinterpret_cast<HelChunk *>(getSysdepsAllocator().allocate(sizeof(HelChunk) + 4096));

		recreateQueue();
	}

	Queue(const Queue &) = delete;

	Queue &operator= (const Queue &) = delete;

	void recreateQueue() {
		// Reset the internal queue state.
		_retrieveIndex = 0;
		_nextIndex = 0;
		_lastProgress = 0;

		// Setup the queue header.
		_queue->headFutex = 0;
		_queue->elementLimit = 128;
		_queue->sizeShift = 1;
		HEL_CHECK(helCreateQueue(_queue, 0, &_handle));
		HEL_CHECK(helSetupChunk(_handle, 0, _chunks[0], 0));
		HEL_CHECK(helSetupChunk(_handle, 1, _chunks[1], 0));

		// Reset and enqueue the chunks.
		_chunks[0]->progressFutex = 0;
		_chunks[1]->progressFutex = 0;
		_refCount[0] = 1;
		_refCount[1] = 1;

		_queue->indexQueue[0] = 0;
		_queue->indexQueue[1] = 1;
		_nextIndex = 2;
		_wakeHeadFutex();
	}

	HelHandle getQueue() {
		return _handle;
	}

	void trim() { }
	
	ElementHandle dequeueSingle() {
		while(true) {
			__ensure(_retrieveIndex != _nextIndex);

			bool done;
			_waitProgressFutex(&done);

			auto n = _numberOf(_retrieveIndex);
			__ensure(_refCount[n]);

			if(done) {
				retire(n);

				_lastProgress = 0;
				_retrieveIndex = ((_retrieveIndex + 1) & kHelHeadMask);
				continue;
			}

			// Dequeue the next element.
			auto ptr = (char *)_chunks[n] + sizeof(HelChunk) + _lastProgress;
			auto element = reinterpret_cast<HelElement *>(ptr);
			_lastProgress += sizeof(HelElement) + element->length;
			_refCount[n]++;
			return ElementHandle{this, n, ptr + sizeof(HelElement)};
		}
	}

	void retire(int n) {
		__ensure(_refCount[n]);
		if(_refCount[n]-- > 1)
			return;

		// Reset and enqueue the chunk again.
		_chunks[n]->progressFutex = 0;
		_refCount[n] = 1;

		_queue->indexQueue[_nextIndex & 1] = n;
		_nextIndex = ((_nextIndex + 1) & kHelHeadMask);
		_wakeHeadFutex();
	}

private:
	int _numberOf(int index) {
		return _queue->indexQueue[index & 1];
	}

	HelChunk *_retrieveChunk() {
		return _chunks[_numberOf(_retrieveIndex)];
	}

	void _wakeHeadFutex() {
		auto futex = __atomic_exchange_n(&_queue->headFutex, _nextIndex, __ATOMIC_RELEASE);
		if(futex & kHelHeadWaiters)
			HEL_CHECK(helFutexWake(&_queue->headFutex));
	}

	void _waitProgressFutex(bool *done) {
		while(true) {
			auto futex = __atomic_load_n(&_retrieveChunk()->progressFutex, __ATOMIC_ACQUIRE);
			do {
				if(_lastProgress != (futex & kHelProgressMask)) {
					*done = false;
					return;
				}else if(futex & kHelProgressDone) {
					*done = true;
					return;
				}

				__ensure(futex == _lastProgress);
			} while(!__atomic_compare_exchange_n(&_retrieveChunk()->progressFutex, &futex,
						_lastProgress | kHelProgressWaiters,
						false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE));
			
			HEL_CHECK(helFutexWait(&_retrieveChunk()->progressFutex,
					_lastProgress | kHelProgressWaiters));
		}
	}

private:
	HelHandle _handle;
	HelQueue *_queue;
	HelChunk *_chunks[2];
	
	// Index of the chunk that we are currently retrieving/inserting next.
	int _retrieveIndex;
	int _nextIndex;

	// Progress into the current chunk.
	int _lastProgress;

	// Number of ElementHandle objects alive.
	int _refCount[2];
};

inline ElementHandle::~ElementHandle() {
	if(_queue)
		_queue->retire(_n);
}

inline HelSimpleResult *parseSimple(ElementHandle &element) {
	auto result = reinterpret_cast<HelSimpleResult *>(element.data());
	element.advance(sizeof(HelSimpleResult));
	return result;
}

inline HelInlineResult *parseInline(ElementHandle &element) {
	auto result = reinterpret_cast<HelInlineResult *>(element.data());
	element.advance(sizeof(HelInlineResult) + ((result->length + 7) & ~size_t(7)));
	return result;
}

inline HelLengthResult *parseLength(ElementHandle &element) {
	auto result = reinterpret_cast<HelLengthResult *>(element.data());
	element.advance(sizeof(HelLengthResult));
	return result;
}

inline HelHandleResult *parseHandle(ElementHandle &element) {
	auto result = reinterpret_cast<HelHandleResult *>(element.data());
	element.advance(sizeof(HelHandleResult));
	return result;
}

HelHandle getPosixLane();
HelHandle *cacheFileTable();
void clearCachedInfos();

extern thread_local Queue globalQueue;

#endif // MLIBC_POSIX_PIPE


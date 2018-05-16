
#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

// FIXME: required for hel.h
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/initializer.hpp>

#include <hel.h>
#include <hel-syscalls.h>

struct Queue;

struct ElementHandle {
	ElementHandle(Queue *queue, void *data)
	: _queue{queue}, _data{data} { }

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
	void *_data;
};

struct Queue {
	Queue()
	: _handle{kHelNullHandle}, _refCount{0} {
		_queue = reinterpret_cast<HelQueue *>(getAllocator().allocate(sizeof(HelQueue)
				+ 2 * sizeof(int)));
		_chunks[0] = reinterpret_cast<HelChunk *>(getAllocator().allocate(sizeof(HelChunk) + 4096));
		_chunks[1] = reinterpret_cast<HelChunk *>(getAllocator().allocate(sizeof(HelChunk) + 4096));

		recreateQueue();
	}

	Queue(const Queue &) = delete;

	Queue &operator= (const Queue &) = delete;

	// TODO: Add this once we turn globalQueue into an accessor function for an Eternal object.
//	~Queue() {
//		__ensure(!_refCount);
//	}

	void recreateQueue() {
		__ensure(!_refCount);

		// Reset the internal queue state.
		_retrieveIndex = 0;
		_nextIndex = 0;
		_lastProgress = 0;

		for(int i = 0; i < 2; i++)
			_requeue[i] = false;

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
		__ensure(!_refCount);

		while(true) {
			__ensure(_retrieveIndex != _nextIndex);

			bool done;
			_waitProgressFutex(&done);
			if(done) {
				__ensure(!_requeue[_numberOf(_retrieveIndex)]);
				_requeue[_numberOf(_retrieveIndex)] = true;

				_lastProgress = 0;
				_retrieveIndex = ((_retrieveIndex + 1) & kHelHeadMask);
				continue;
			}

			// Dequeue the next element.
			auto ptr = (char *)_retrieveChunk() + sizeof(HelChunk) + _lastProgress;
			auto element = reinterpret_cast<HelElement *>(ptr);
			_lastProgress += sizeof(HelElement) + element->length;
			_refCount++;
			return ElementHandle{this, ptr + sizeof(HelElement)};
		}
	}

	void retire() {
		__ensure(_refCount > 0);
		if(_refCount-- > 1)
			return;

		for(int i = 0; i < 2; i++) {
			if(!_requeue[i])
				continue;

			// Reset and enqueue the chunk again.
			_chunks[i]->progressFutex = 0;
			
			_queue->indexQueue[_nextIndex & 1] = i;
			_nextIndex = ((_nextIndex + 1) & kHelHeadMask);
			_wakeHeadFutex();
			_requeue[i] = false;
		}
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
	int _refCount;

	// Stores for each chunk, if it is available for requeuing.
	bool _requeue[2];
};

inline ElementHandle::~ElementHandle() {
	if(_queue)
		_queue->retire();
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

HelHandle *cacheFileTable();
void clearCachedInfos();

extern thread_local Queue globalQueue;

#endif // MLIBC_POSIX_PIPE


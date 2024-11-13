#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

#include <cstddef>
#include <signal.h>
#include <stdint.h>
#include <string.h>

#include <hel-syscalls.h>
#include <hel.h>

struct SignalGuard {
	SignalGuard();

	SignalGuard(const SignalGuard &) = delete;

	~SignalGuard();

	SignalGuard &operator=(const SignalGuard &) = delete;
};

// We need an allocator for message structs in sysdeps functions; the "normal" mlibc
// allocator cannot be used, as the sysdeps function might be called from a signal.
MemoryAllocator &getSysdepsAllocator();

struct Queue;

struct ElementHandle {
	friend void swap(ElementHandle &u, ElementHandle &v) {
		using std::swap;
		swap(u._queue, v._queue);
		swap(u._n, v._n);
		swap(u._data, v._data);
	}

	ElementHandle() : _queue{nullptr}, _n{-1}, _data{nullptr} {}

	ElementHandle(Queue *queue, int n, void *data) : _queue{queue}, _n{n}, _data{data} {}

	ElementHandle(const ElementHandle &other);

	ElementHandle(ElementHandle &&other) : ElementHandle{} { swap(*this, other); }

	~ElementHandle();

	ElementHandle &operator=(ElementHandle other) {
		swap(*this, other);
		return *this;
	}

	void *data() { return _data; }

	void advance(size_t size) { _data = reinterpret_cast<char *>(_data) + size; }

private:
	Queue *_queue;
	int _n;
	void *_data;
};

struct Queue {
	Queue() : _handle{kHelNullHandle} {
		// We do not need to protect those allocations against signals as this constructor
		// is only called during library initialization.
		_chunks[0] =
		    reinterpret_cast<HelChunk *>(getSysdepsAllocator().allocate(sizeof(HelChunk) + 4096));
		_chunks[1] =
		    reinterpret_cast<HelChunk *>(getSysdepsAllocator().allocate(sizeof(HelChunk) + 4096));

		recreateQueue();
	}

	Queue(const Queue &) = delete;

	Queue &operator=(const Queue &) = delete;

	void recreateQueue() {
		// Reset the internal queue state.
		_retrieveIndex = 0;
		_nextIndex = 0;
		_lastProgress = 0;

		// Setup the queue header.
		HelQueueParameters params{.flags = 0, .ringShift = 1, .numChunks = 2, .chunkSize = 4096};
		HEL_CHECK(helCreateQueue(&params, &_handle));

		auto chunksOffset = (sizeof(HelQueue) + (sizeof(int) << 1) + 63) & ~size_t(63);
		auto reservedPerChunk = (sizeof(HelChunk) + params.chunkSize + 63) & ~size_t(63);
		auto overallSize = chunksOffset + params.numChunks * reservedPerChunk;

		void *mapping;
		HEL_CHECK(helMapMemory(
		    _handle,
		    kHelNullHandle,
		    nullptr,
		    0,
		    (overallSize + 0xFFF) & ~size_t(0xFFF),
		    kHelMapProtRead | kHelMapProtWrite,
		    &mapping
		));

		_queue = reinterpret_cast<HelQueue *>(mapping);
		auto chunksPtr = reinterpret_cast<std::byte *>(mapping) + chunksOffset;
		for (unsigned int i = 0; i < 2; ++i)
			_chunks[i] = reinterpret_cast<HelChunk *>(chunksPtr + i * reservedPerChunk);

		// Reset and enqueue the chunks.
		_chunks[0]->progressFutex = 0;
		_chunks[1]->progressFutex = 0;
		_refCount[0] = 1;
		_refCount[1] = 1;

		_queue->indexQueue[0] = 0;
		_queue->indexQueue[1] = 1;
		_queue->headFutex = 0;
		_nextIndex = 2;
		_wakeHeadFutex();
	}

	HelHandle getQueue() { return _handle; }

	void trim() {}

	ElementHandle dequeueSingle() {
		while (true) {
			__ensure(_retrieveIndex != _nextIndex);

			bool done;
			_waitProgressFutex(&done);

			auto n = _numberOf(_retrieveIndex);
			__ensure(_refCount[n]);

			if (done) {
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
		if (_refCount[n]-- > 1)
			return;

		// Reset and enqueue the chunk again.
		_chunks[n]->progressFutex = 0;
		_refCount[n] = 1;

		_queue->indexQueue[_nextIndex & 1] = n;
		_nextIndex = ((_nextIndex + 1) & kHelHeadMask);
		_wakeHeadFutex();
	}

	void reference(int n) { _refCount[n]++; }

private:
	int _numberOf(int index) { return _queue->indexQueue[index & 1]; }

	HelChunk *_retrieveChunk() { return _chunks[_numberOf(_retrieveIndex)]; }

	void _wakeHeadFutex() {
		auto futex = __atomic_exchange_n(&_queue->headFutex, _nextIndex, __ATOMIC_RELEASE);
		if (futex & kHelHeadWaiters)
			HEL_CHECK(helFutexWake(&_queue->headFutex));
	}

	void _waitProgressFutex(bool *done) {
		while (true) {
			auto futex = __atomic_load_n(&_retrieveChunk()->progressFutex, __ATOMIC_ACQUIRE);
			__ensure(!(futex & ~(kHelProgressMask | kHelProgressWaiters | kHelProgressDone)));
			do {
				if (_lastProgress != (futex & kHelProgressMask)) {
					*done = false;
					return;
				} else if (futex & kHelProgressDone) {
					*done = true;
					return;
				}

				if (futex & kHelProgressWaiters)
					break; // Waiters bit is already set (in a previous iteration).
			} while (!__atomic_compare_exchange_n(
			    &_retrieveChunk()->progressFutex,
			    &futex,
			    _lastProgress | kHelProgressWaiters,
			    false,
			    __ATOMIC_ACQUIRE,
			    __ATOMIC_ACQUIRE
			));

			HEL_CHECK(helFutexWait(
			    &_retrieveChunk()->progressFutex, _lastProgress | kHelProgressWaiters, -1
			));
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
	if (_queue)
		_queue->retire(_n);
}

inline ElementHandle::ElementHandle(const ElementHandle &other) {
	_queue = other._queue;
	_n = other._n;
	_data = other._data;

	_queue->reference(_n);
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
HelHandle getHandleForFd(int fd);
void clearCachedInfos();

extern thread_local Queue globalQueue;

// This include is here because it needs ElementHandle to be declared
#include <helix/ipc-structs.hpp>

template <typename... Args>
auto exchangeMsgsSync(HelHandle descriptor, Args &&...args) {
	auto results = helix_ng::createResultsTuple(args...);
	auto actions = helix_ng::chainActionArrays(args...);

	HEL_CHECK(
	    helSubmitAsync(descriptor, actions.data(), actions.size(), globalQueue.getQueue(), 0, 0)
	);

	auto element = globalQueue.dequeueSingle();
	void *ptr = element.data();

	[&]<size_t... p>(std::index_sequence<p...>) {
		(results.template get<p>().parse(ptr, element), ...);
	}(std::make_index_sequence<std::tuple_size_v<decltype(results)>>{});

	return results;
}

#endif // MLIBC_POSIX_PIPE

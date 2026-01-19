#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

#include <cstddef>
#include <string.h>

#include <hel-syscalls.h>
#include <hel.h>

#include <bits/ensure.h>
#include <frg/array.hpp>
#include <frg/optional.hpp>
#include <frg/span.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <protocols/posix/data.hpp>
#include <protocols/posix/supercalls.hpp>

void resetCancellationRequested();
void setQueueHandle(HelHandle queue);

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
	Queue() : _handle{kHelNullHandle}, _numSqChunks{2} {
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
		_retrieveChunk = 0;
		_tailChunk = 0;
		_lastProgress = 0;

		// Setup the queue header.
		HelQueueParameters params{.flags = 0, .numChunks = 2, .chunkSize = 4096, .numSqChunks = _numSqChunks};
		HEL_CHECK(helCreateQueue(&params, &_handle));
		setQueueHandle(_handle);

		auto totalChunks = params.numChunks + params.numSqChunks;
		auto chunksOffset = (sizeof(HelQueue) + 63) & ~size_t(63);
		auto reservedPerChunk = (sizeof(HelChunk) + params.chunkSize + 63) & ~size_t(63);
		auto overallSize = chunksOffset + totalChunks * reservedPerChunk;

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

		// Reset all CQ chunks.
		for (unsigned int i = 0; i < 2; ++i) {
			_chunks[i]->next = 0;
			_chunks[i]->progressFutex = 0;
			_refCount[i] = 1;
		}

		// Set cqFirst to the initial chunk.
		__atomic_store_n(&_queue->cqFirst, 0 | kHelNextPresent, __ATOMIC_RELEASE);

		// Supply the remaining CQ chunks.
		_tailChunk = 0;
		for (unsigned int i = 1; i < 2; ++i) {
			__atomic_store_n(&_chunks[_tailChunk]->next, i | kHelNextPresent, __ATOMIC_RELEASE);
			_tailChunk = i;
		}
		_wakeHeadFutex();

		// Initialize SQ state if we have SQ chunks.
		if (_numSqChunks > 0) {
			// Store pointers to SQ chunks.
			for (unsigned int i = 0; i < _numSqChunks; ++i) {
				_sqChunks[i] = reinterpret_cast<HelChunk *>(
				    chunksPtr + (2 + i) * reservedPerChunk);
			}

			// SQ is initialized by the kernel. Read sqFirst to get the first SQ chunk.
			auto sqFirst = __atomic_load_n(&_queue->sqFirst, __ATOMIC_ACQUIRE);
			_sqCurrentChunk = sqFirst & ~kHelNextPresent;
			_sqProgress = 0;
			_chunkSize = params.chunkSize;
		}
	}

	HelHandle getQueue() { return _handle; }

	void trim() {}

	// Push an element to the submission queue using a gather list.
	void pushSq(uint32_t opcode, uintptr_t context,
			frg::span<const frg::span<const std::byte>> segments) {
		__ensure(_numSqChunks > 0);

		size_t dataLength = 0;
		for (auto seg : segments)
			dataLength += seg.size();

		auto elementSize = sizeof(HelElement) + dataLength;

		// Check if we need to move to the next SQ chunk.
		if (_sqProgress + elementSize > _chunkSize) {
			// Wait for next chunk to become available.
			auto nextWord = __atomic_load_n(&_sqChunks[_sqCurrentChunk - 2]->next, __ATOMIC_ACQUIRE);
			while(!(nextWord & kHelNextPresent)) {
				__atomic_fetch_and(&_queue->userNotify, ~kHelUserNotifySupplySqChunks, __ATOMIC_ACQUIRE);

				nextWord = __atomic_load_n(&_sqChunks[_sqCurrentChunk - 2]->next, __ATOMIC_ACQUIRE);
				if(nextWord & kHelNextPresent)
					break;

				HEL_CHECK(helDriveQueue(_handle, 0));
			}

			// Mark current chunk as done.
			__atomic_store_n(&_sqChunks[_sqCurrentChunk - 2]->progressFutex,
			                 _sqProgress | kHelProgressDone, __ATOMIC_RELEASE);

			// Signal the kernel.
			// Note: We do not call helDriveQueue() here; instead this is done at the next dequeue.
			__atomic_fetch_or(&_queue->kernelNotify, kHelKernelNotifySqProgress, __ATOMIC_RELEASE);

			_sqCurrentChunk = nextWord & ~kHelNextPresent;
			_sqProgress = 0;
		}

		// Write the element to the SQ chunk.
		auto ptr = reinterpret_cast<char *>(_sqChunks[_sqCurrentChunk - 2]) +
		           sizeof(HelChunk) + _sqProgress;
		auto element = reinterpret_cast<HelElement *>(ptr);
		element->length = dataLength;
		element->opcode = opcode;
		element->context = reinterpret_cast<void *>(context);

		// Copy each segment.
		size_t offset = 0;
		for (auto seg : segments) {
			memcpy(ptr + sizeof(HelElement) + offset, seg.data(), seg.size());
			offset += seg.size();
		}

		_sqProgress += elementSize;

		// Update the progress futex.
		__atomic_store_n(&_sqChunks[_sqCurrentChunk - 2]->progressFutex, _sqProgress, __ATOMIC_RELEASE);

		// Signal the kernel.
		// Note: We do not call helDriveQueue() here; instead this is done at the next dequeue.
		__atomic_fetch_or(&_queue->kernelNotify, kHelKernelNotifySqProgress, __ATOMIC_RELEASE);
	}

	frg::optional<ElementHandle> dequeueSingleUnlessCancelled() {
		while (true) {
			auto progress = _waitProgressFutex();

			auto n = _retrieveChunk;
			__ensure(_refCount[n]);

			if (progress == FutexProgress::DONE) {
				auto next = __atomic_load_n(&_chunks[n]->next, __ATOMIC_ACQUIRE);
				retire(n);

				_lastProgress = 0;
				// Otherwise, the kernel would not have set the done bit.
				__ensure(next & kHelNextPresent);
				_retrieveChunk = next & ~kHelNextPresent;
				continue;
			}

			if (progress == FutexProgress::CANCELLED)
				return frg::null_opt;

			// Dequeue the next element.
			auto ptr = (char *)_chunks[n] + sizeof(HelChunk) + _lastProgress;
			auto element = reinterpret_cast<HelElement *>(ptr);
			_lastProgress += sizeof(HelElement) + element->length;
			_refCount[n]++;
			return ElementHandle{this, n, ptr + sizeof(HelElement)};
		}
	}

	ElementHandle dequeueSingle() {
		while (true) {
			auto result = dequeueSingleUnlessCancelled();
			if (result)
				return *result;
			else
				resetCancellationRequested();
		}
	}

	void retire(int n) {
		__ensure(_refCount[n]);
		if (_refCount[n]-- > 1)
			return;

		// Reset and supply the chunk again.
		_chunks[n]->next = 0;
		_chunks[n]->progressFutex = 0;
		_refCount[n] = 1;

		__atomic_store_n(&_chunks[_tailChunk]->next, n | kHelNextPresent, __ATOMIC_RELEASE);
		_tailChunk = n;
		_wakeHeadFutex();
	}

	void reference(int n) { _refCount[n]++; }

private:
	void _wakeHeadFutex() {
		auto futex =
		    __atomic_fetch_or(&_queue->kernelNotify, kHelKernelNotifySupplyCqChunks, __ATOMIC_RELEASE);
		if (!(futex & kHelKernelNotifySupplyCqChunks))
			HEL_CHECK(helDriveQueue(_handle, 0));
	}

	enum class FutexProgress {
		NONE,
		DONE,
		PROGRESS,
		CANCELLED,
	};

	// Postcondition: return value != FutexProgress::NONE.
	FutexProgress _waitProgressFutex() {
		auto check = [&]() -> FutexProgress {
			auto progress = __atomic_load_n(&_chunks[_retrieveChunk]->progressFutex, __ATOMIC_ACQUIRE);
			__ensure(!(progress & ~(kHelProgressMask | kHelProgressDone)));
			if (_lastProgress != (progress & kHelProgressMask))
				return FutexProgress::PROGRESS;
			else if (progress & kHelProgressDone)
				return FutexProgress::DONE;
			return FutexProgress::NONE;
		};

		if (auto result = check(); result != FutexProgress::NONE)
			return result;

		while (true) {
			auto fetch = __atomic_fetch_and(&_queue->userNotify, ~(kHelUserNotifyCqProgress | kHelUserNotifyAlert), __ATOMIC_ACQUIRE);
			if (fetch & kHelUserNotifyAlert)
				return FutexProgress::CANCELLED;

			if (auto result = check(); result != FutexProgress::NONE)
				return result;

			int err = helDriveQueue(_handle, kHelDriveWaitCqProgress);

			if (err == kHelErrCancelled)
				return FutexProgress::CANCELLED;

			HEL_CHECK(err);
		}
	}

private:
	HelHandle _handle;
	HelQueue *_queue;
	HelChunk *_chunks[2];

	// Chunk that we are currently retrieving from.
	int _retrieveChunk;
	// Tail of the chunk list (where we append new chunks).
	int _tailChunk;

	// Progress into the current chunk.
	int _lastProgress;

	// Number of ElementHandle objects alive.
	int _refCount[2];

	// Number of SQ chunks configured.
	unsigned int _numSqChunks;

	// SQ state (only used if _numSqChunks > 0).
	HelChunk *_sqChunks[2];  // Max 2 SQ chunks for now.
	int _sqCurrentChunk;
	int _sqProgress;
	size_t _chunkSize;
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
void resetCancellationId();
void setCancellationId(uint64_t event, HelHandle handle, int fd);
bool cancellationRequested();
void clearCachedInfos();
uint64_t allocateCancellationId();

extern thread_local Queue globalQueue;

// This include is here because it needs ElementHandle to be declared
#include <helix/ipc-structs.hpp>

template <typename... Args>
auto exchangeMsgsSync(HelHandle descriptor, Args &&...args) {
	auto results = helix_ng::createResultsTuple(args...);
	auto actions = helix_ng::chainActionArrays(args...);

	HelSqExchangeMsgs header;
	header.lane = descriptor;
	header.count = actions.size();
	header.flags = 0;

	frg::array<frg::span<const std::byte>, 2> segments{
		frg::span<const std::byte>{reinterpret_cast<const std::byte *>(&header), sizeof(header)},
		frg::span<const std::byte>{reinterpret_cast<const std::byte *>(actions.data()),
		                           actions.size() * sizeof(HelAction)}
	};
	globalQueue.pushSq(kHelSubmitExchangeMsgs, 0, segments);

	auto element = globalQueue.dequeueSingle();
	void *ptr = element.data();

	[&]<size_t... p>(std::index_sequence<p...>) {
		(results.template get<p>().parse(ptr, element), ...);
	}(std::make_index_sequence<std::tuple_size_v<decltype(results)>>{});

	return results;
}

template <typename... Args>
auto exchangeMsgsSyncCancellable(HelHandle descriptor, uint64_t cancelId, int fd, Args &&...args) {
	auto results = helix_ng::createResultsTuple(args...);
	auto actions = helix_ng::chainActionArrays(args...);

	HelSqExchangeMsgs header;
	header.lane = descriptor;
	header.count = actions.size();
	header.flags = 0;

	frg::array<frg::span<const std::byte>, 2> segments{
		frg::span<const std::byte>{reinterpret_cast<const std::byte *>(&header), sizeof(header)},
		frg::span<const std::byte>{reinterpret_cast<const std::byte *>(actions.data()),
		                           actions.size() * sizeof(HelAction)}
	};
	globalQueue.pushSq(kHelSubmitExchangeMsgs, 0, segments);

	frg::optional<ElementHandle> element{};

	do {
		element = globalQueue.dequeueSingleUnlessCancelled();

		if (cancellationRequested()) {
			HEL_CHECK(helSyscall2(
				kHelCallSuper + posix::superCancel,
				cancelId,
				fd
			));
			resetCancellationRequested();
		}
	} while (!element);

	void *ptr = element->data();

	[&]<size_t... p>(std::index_sequence<p...>) {
		(results.template get<p>().parse(ptr, *element), ...);
	}(std::make_index_sequence<std::tuple_size_v<decltype(results)>>{});

	return results;
}

#endif // MLIBC_POSIX_PIPE

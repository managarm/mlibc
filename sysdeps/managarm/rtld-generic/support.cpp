#include <cstddef>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#include <frg/manual_box.hpp>
#include <frg/string.hpp>
#include <hel-syscalls.h>
#include <hel.h>

#include <fs.frigg_bragi.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <posix.frigg_bragi.hpp>

#include <protocols/posix/data.hpp>
#include <protocols/posix/supercalls.hpp>

// --------------------------------------------------------
// POSIX I/O functions.
// --------------------------------------------------------

HelHandle posixLane;
HelHandle *fileTable;

extern "C" [[gnu::visibility("hidden")]] void abort() {
	mlibc::panicLogger() << "rtld: abort() called" << frg::endlog;
	__builtin_unreachable();
}

void cacheFileTable() {
	if (fileTable)
		return;

	posix::ManagarmProcessData data;
	HEL_CHECK(
	    helSyscall1(kHelCallSuper + posix::superGetProcessData, reinterpret_cast<HelWord>(&data))
	);
	posixLane = data.posixLane;
	fileTable = data.fileTable;
}

template <typename T>
T load(void *ptr) {
	T result;
	memcpy(&result, ptr, sizeof(T));
	return result;
}

// This Queue implementation is more simplistic than the ones in mlibc and helix.
// In fact, we only manage a single chunk; this minimizes the memory usage of the queue.
struct Queue {
	Queue() : _handle{kHelNullHandle}, _lastProgress(0) {
		HelQueueParameters params{.flags = 0, .ringShift = 0, .numChunks = 1, .chunkSize = 4096};
		HEL_CHECK(helCreateQueue(&params, &_handle));

		auto chunksOffset = (sizeof(HelQueue) + (sizeof(int) << 0) + 63) & ~size_t(63);
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
		_chunk =
		    reinterpret_cast<HelChunk *>(reinterpret_cast<std::byte *>(mapping) + chunksOffset);

		// Reset and enqueue the first chunk.
		_chunk->progressFutex = 0;

		_queue->indexQueue[0] = 0;
		_queue->headFutex = 1;
		_nextIndex = 1;
		_wakeHeadFutex();
	}

	Queue(const Queue &) = delete;

	Queue &operator=(const Queue &) = delete;

	HelHandle getHandle() { return _handle; }

	void *dequeueSingle() {
		while (true) {
			bool done;
			_waitProgressFutex(&done);
			if (done) {
				// Reset and enqueue the chunk again.
				_chunk->progressFutex = 0;

				_queue->indexQueue[0] = 0;
				_nextIndex = ((_nextIndex + 1) & kHelHeadMask);
				_wakeHeadFutex();

				_lastProgress = 0;
				continue;
			}

			// Dequeue the next element.
			auto ptr = (char *)_chunk + sizeof(HelChunk) + _lastProgress;
			auto element = load<HelElement>(ptr);
			_lastProgress += sizeof(HelElement) + element.length;
			return ptr + sizeof(HelElement);
		}
	}

private:
	void _wakeHeadFutex() {
		auto futex = __atomic_exchange_n(&_queue->headFutex, _nextIndex, __ATOMIC_RELEASE);
		if (futex & kHelHeadWaiters)
			HEL_CHECK(helFutexWake(&_queue->headFutex));
	}

	void _waitProgressFutex(bool *done) {
		while (true) {
			auto futex = __atomic_load_n(&_chunk->progressFutex, __ATOMIC_ACQUIRE);
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
			    &_chunk->progressFutex,
			    &futex,
			    _lastProgress | kHelProgressWaiters,
			    false,
			    __ATOMIC_ACQUIRE,
			    __ATOMIC_ACQUIRE
			));

			HEL_CHECK(helFutexWait(&_chunk->progressFutex, _lastProgress | kHelProgressWaiters, -1)
			);
		}
	}

private:
	HelHandle _handle;
	HelQueue *_queue;
	HelChunk *_chunk;
	int _nextIndex;
	int _lastProgress;
};

frg::manual_box<Queue> globalQueue;

HelSimpleResult *parseSimple(void *&element) {
	auto result = reinterpret_cast<HelSimpleResult *>(element);
	element = (char *)element + sizeof(HelSimpleResult);
	return result;
}

HelInlineResult *parseInline(void *&element) {
	auto result = reinterpret_cast<HelInlineResult *>(element);
	element = (char *)element + sizeof(HelInlineResult) + ((result->length + 7) & ~size_t(7));
	return result;
}

HelLengthResult *parseLength(void *&element) {
	auto result = reinterpret_cast<HelLengthResult *>(element);
	element = (char *)element + sizeof(HelLengthResult);
	return result;
}

HelHandleResult *parseHandle(void *&element) {
	auto result = reinterpret_cast<HelHandleResult *>(element);
	element = (char *)element + sizeof(HelHandleResult);
	return result;
}

namespace mlibc {

int sys_tcb_set(void *pointer) {
#if defined(__x86_64__)
	HEL_CHECK(helWriteFsBase(pointer));
#elif defined(__aarch64__)
	uintptr_t addr = reinterpret_cast<uintptr_t>(pointer);
	addr += sizeof(Tcb) - 0x10;
	asm volatile("msr tpidr_el0, %0" ::"r"(addr));
#elif defined(__riscv) && __riscv_xlen == 64
	uintptr_t tp = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" : : "r"(tp) : "memory");
#else
#error Unknown architecture
#endif
	return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	cacheFileTable();
	HelAction actions[4];

	managarm::posix::OpenAtRequest<MemoryAllocator> req(getAllocator());
	req.set_fd(AT_FDCWD);
	req.set_flags(flags);
	req.set_mode(mode);
	req.set_path(frg::string<MemoryAllocator>(getAllocator(), path));

	if (!globalQueue.valid())
		globalQueue.initialize();

	frg::string<MemoryAllocator> head(getAllocator());
	frg::string<MemoryAllocator> tail(getAllocator());
	head.resize(req.size_of_head());
	tail.resize(req.size_of_tail());
	bragi::limited_writer headWriter{head.data(), head.size()};
	bragi::limited_writer tailWriter{tail.data(), tail.size()};
	auto headOk = req.encode_head(headWriter);
	auto tailOk = req.encode_tail(tailWriter);
	__ensure(headOk);
	__ensure(tailOk);

	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = head.data();
	actions[1].length = head.size();
	actions[2].type = kHelActionSendFromBuffer;
	actions[2].flags = kHelItemChain;
	actions[2].buffer = tail.data();
	actions[2].length = tail.size();
	actions[3].type = kHelActionRecvInline;
	actions[3].flags = 0;
	HEL_CHECK(helSubmitAsync(posixLane, actions, 4, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseHandle(element);
	auto send_head = parseSimple(element);
	auto send_tail = parseSimple(element);
	auto recv_resp = parseInline(element);
	HEL_CHECK(offer->error);
	HEL_CHECK(send_head->error);
	HEL_CHECK(send_tail->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);

	if (resp.error() == managarm::posix::Errors::FILE_NOT_FOUND)
		return -1;
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*fd = resp.fd();
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	__ensure(whence == SEEK_SET);

	cacheFileTable();
	auto lane = fileTable[fd];
	HelAction actions[3];

	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::SEEK_ABS);
	req.set_rel_offset(offset);

	if (!globalQueue.valid())
		globalQueue.initialize();

	frg::string<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(lane, actions, 3, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	*new_offset = offset;
	return 0;
}

int sys_read(int fd, void *data, size_t length, ssize_t *bytes_read) {
	cacheFileTable();
	auto lane = fileTable[fd];
	HelAction actions[5];

	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::READ);
	req.set_size(length);

	if (!globalQueue.valid())
		globalQueue.initialize();

	frg::string<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionImbueCredentials;
	actions[2].handle = kHelThisThread;
	actions[2].flags = kHelItemChain;
	actions[3].type = kHelActionRecvInline;
	actions[3].flags = kHelItemChain;
	actions[4].type = kHelActionRecvToBuffer;
	actions[4].flags = 0;
	actions[4].buffer = data;
	actions[4].length = length;
	HEL_CHECK(helSubmitAsync(lane, actions, 5, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);
	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_data->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	*bytes_read = recv_data->length;
	return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	cacheFileTable();
	HelAction actions[3];

	managarm::posix::VmMapRequest<MemoryAllocator> req(getAllocator());
	req.set_address_hint(reinterpret_cast<uintptr_t>(hint));
	req.set_size(size);
	req.set_mode(prot);
	req.set_flags(flags);
	req.set_fd(fd);
	req.set_rel_offset(offset);

	if (!globalQueue.valid())
		globalQueue.initialize();

	frg::string<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(posixLane, actions, 3, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_close(int fd) {
	cacheFileTable();
	HelAction actions[3];

	managarm::posix::CloseRequest<MemoryAllocator> req(getAllocator());
	req.set_fd(fd);

	if (!globalQueue.valid())
		globalQueue.initialize();

	frg::string<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(posixLane, actions, 3, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return 0;
}

int sys_futex_tid() {
	HelWord tid = 0;
	HEL_CHECK(helSyscall0_1(kHelCallSuper + posix::superGetTid, &tid));

	return tid;
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	// This implementation is inherently signal-safe.
	if (time) {
		if (helFutexWait(pointer, expected, time->tv_nsec + time->tv_sec * 1000000000))
			return -1;
		return 0;
	}
	if (helFutexWait(pointer, expected, -1))
		return -1;
	return 0;
}

int sys_futex_wake(int *pointer) {
	// This implementation is inherently signal-safe.
	if (helFutexWake(pointer))
		return -1;
	return 0;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_PROTECT);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);
	req.set_mode(prot);

	if (!globalQueue.valid())
		globalQueue.initialize();

	frg::string<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);

	HelAction actions[3];
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(posixLane, actions, 3, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return 0;
}

} // namespace mlibc


#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frg/manual_box.hpp>
#include <frg/string.hpp>
#include <hel.h>
#include <hel-syscalls.h>

#include <mlibc/allocator.hpp>
#include <mlibc/sysdeps.hpp>
#include <posix.frigg_pb.hpp>
#include <fs.frigg_pb.hpp>

// The frigg protobuf implementation uses assert(), so we need this function.
extern "C" void __assert_fail(const char *assertion,
		const char *file, unsigned int line, const char *function) {
	frg_panic(assertion);
}

// --------------------------------------------------------
// POSIX I/O functions.
// --------------------------------------------------------

HelHandle *fileTable;

void cacheFileTable() {
	if(fileTable)
		return;

	struct managarm_process_data {
		HelHandle posix_lane;
		HelHandle *file_table;
		void *clock_tracker_page;
	};

	managarm_process_data data;
	HEL_CHECK(helSyscall1(kHelCallSuper + 1, reinterpret_cast<HelWord>(&data)));
	fileTable = data.file_table;
}

template<typename T>
T load(void *ptr) {
	T result;
	memcpy(&result, ptr, sizeof(T));
	return result;
}

// This Queue implementation is more simplistic than the ones in mlibc and helix.
// In fact, we only manage a single chunk; this minimizes the memory usage of the queue.
struct Queue {
	Queue()
	: _handle{kHelNullHandle}, _lastProgress(0) {
		_queue = reinterpret_cast<HelQueue *>(getAllocator().allocate(sizeof(HelQueue)
				+ sizeof(int)));
		_queue->headFutex = 1;
		_queue->elementLimit = 128;
		_queue->sizeShift = 0;
		HEL_CHECK(helCreateQueue(_queue, 0, &_handle));

		_chunk = reinterpret_cast<HelChunk *>(getAllocator().allocate(sizeof(HelChunk) + 4096));
		HEL_CHECK(helSetupChunk(_handle, 0, _chunk, 0));

		// Reset and enqueue the first chunk.
		_chunk->progressFutex = 0;

		_queue->indexQueue[0] = 0;
		_nextIndex = 1;
		_wakeHeadFutex();
	}

	Queue(const Queue &) = delete;

	Queue &operator= (const Queue &) = delete;

	HelHandle getHandle() {
		return _handle;
	}

	void *dequeueSingle() {
		while(true) {
			bool done;
			_waitProgressFutex(&done);
			if(done) {
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
		if(futex & kHelHeadWaiters)
			HEL_CHECK(helFutexWake(&_queue->headFutex));
	}

	void _waitProgressFutex(bool *done) {
		while(true) {
			auto futex = __atomic_load_n(&_chunk->progressFutex, __ATOMIC_ACQUIRE);
			do {
				if(_lastProgress != (futex & kHelProgressMask)) {
					*done = false;
					return;
				}else if(futex & kHelProgressDone) {
					*done = true;
					return;
				}

				__ensure(futex == _lastProgress);
			} while(!__atomic_compare_exchange_n(&_chunk->progressFutex, &futex,
						_lastProgress | kHelProgressWaiters,
						false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE));

			HEL_CHECK(helFutexWait(&_chunk->progressFutex,
					_lastProgress | kHelProgressWaiters));
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
	element = (char *)element + sizeof(HelInlineResult)
			+ ((result->length + 7) & ~size_t(7));
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
	HEL_CHECK(helWriteFsBase(pointer));
	return 0;
}

int sys_open(const char *path, int flags, int *fd) {
	HelAction actions[3];

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::OPEN);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

	if(!globalQueue.valid())
		globalQueue.initialize();

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 3, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);

	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND)
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

	if(!globalQueue.valid())
		globalQueue.initialize();

	frigg::String<MemoryAllocator> ser(getAllocator());
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
	auto offer = parseSimple(element);
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

	if(!globalQueue.valid())
		globalQueue.initialize();

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionImbueCredentials;
	actions[2].flags = kHelItemChain;
	actions[3].type = kHelActionRecvInline;
	actions[3].flags = kHelItemChain;
	actions[4].type = kHelActionRecvToBuffer;
	actions[4].flags = 0;
	actions[4].buffer = data;
	actions[4].length = length;
	HEL_CHECK(helSubmitAsync(lane, actions, 5, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseSimple(element);
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
	HelHandle handle;
	if(!(flags & MAP_ANONYMOUS)) {
		cacheFileTable();
		auto lane = fileTable[fd];

		HelAction actions[4];

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::MMAP);

		if(!globalQueue.valid())
			globalQueue.initialize();

		frigg::String<MemoryAllocator> ser(getAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionRecvInline;
		actions[2].flags = kHelItemChain;
		actions[3].type = kHelActionPullDescriptor;
		actions[3].flags = 0;
		HEL_CHECK(helSubmitAsync(lane, actions, 4, globalQueue->getHandle(), 0, 0));

		auto element = globalQueue->dequeueSingle();
		auto offer = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);
		auto pull_memory = parseHandle(element);
		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);
		HEL_CHECK(pull_memory->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		handle = pull_memory->handle;
	}else{
		__ensure(fd == -1);
		__ensure(!offset);
		if(!size) // helAllocateMemory() does not like zero sizes.
			return 0;
		HEL_CHECK(helAllocateMemory(size, 0, &handle));
	}

	__ensure(size);

	uint32_t hel_flags = 0;
	if(prot & PROT_READ)
		hel_flags |= kHelMapProtRead;
	if(prot & PROT_WRITE)
		hel_flags |= kHelMapProtWrite;
	if(prot & PROT_EXEC)
		hel_flags |= kHelMapProtExecute;

	void *map_pointer;
	HEL_CHECK(helMapMemory(handle, kHelNullHandle,
			hint, offset, size,
			hel_flags | kHelMapCopyOnWriteAtFork,
			&map_pointer));
	HEL_CHECK(helCloseDescriptor(handle));
	*window = map_pointer;
	return 0;
}

int sys_close(int fd) {
	HelAction actions[3];

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::CLOSE);
	req.set_fd(fd);

	if(!globalQueue.valid())
		globalQueue.initialize();

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 3, globalQueue->getHandle(), 0, 0));

	auto element = globalQueue->dequeueSingle();
	auto offer = parseSimple(element);
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


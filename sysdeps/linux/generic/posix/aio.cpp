#include <linux/futex.h>
#include <linux/io_uring.h>

#include <abi-bits/fcntl.h>
#include <aio.h>
#include <bits/ensure.h>
#include <errno.h>
#include <expected>
#include <fcntl.h>
#include <frg/intrusive.hpp>
#include <frg/list.hpp>
#include <frg/scope_exit.hpp>
#include <frg/vector.hpp>
#include <limits.h>
#include <mlibc-config.h>
#include <mlibc/aio.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/threads.hpp>
#include <mlibc/time-helpers.hpp>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <type_traits>

#include "../cxx-syscall.hpp"

namespace mlibc {

namespace {

// We use the lowest bits of user_data to signify a cancellation request
// This is safe as `struct aiocb` is at least uintptr_t-aligned.
constexpr uintptr_t cancelBit = 1 << 0;
// Use bit 1 for signifying cancellation requests for all operations on an fd.
constexpr uintptr_t cancelFdBit = 1 << 1;

constexpr uintptr_t userDataBits = (cancelBit | cancelFdBit);
constexpr uintptr_t userDataMask = ~(cancelBit | cancelFdBit);

static_assert(userDataBits < alignof(struct aiocb));

struct io_sq_ring {
	unsigned *head;
	unsigned *tail;
	unsigned *ring_mask;
	unsigned *ring_entries;
	unsigned *flags;
	unsigned *array;
};

struct io_cq_ring {
	unsigned *head;
	unsigned *tail;
	unsigned *ring_mask;
	unsigned *ring_entries;
	struct io_uring_cqe *cqes;
};

struct io_uring_instance {
	int fd;
	io_sq_ring sq;
	io_cq_ring cq;
	io_uring_sqe *sqes;

	void setup();

	void register_eventfd(int evfd) {
		auto ret = do_syscall(SYS_io_uring_register, fd, IORING_REGISTER_EVENTFD, &evfd, 1);
		__ensure(!sc_error(ret));
	}

	std::expected<int, int> enter(
	    unsigned int to_submit, unsigned int min_complete, unsigned int flags, const sigset_t *sig
	);

	template <typename F>
	std::expected<int, int> enqueue(F func) {
		unsigned tail, mask, index;

		// Get SQ tail
		tail = *sq.tail;
		mask = *sq.ring_mask;
		index = tail & mask;

		// Get SQE entry
		struct io_uring_sqe *sqe = &sqes[index];
		memset(sqe, 0, sizeof(*sqe));

		func(sqe);

		sq.array[index] = index;
		__atomic_store_n(sq.tail, tail + 1, __ATOMIC_RELEASE);

		return enter(1, 0, 0, nullptr);
	}
};

constexpr size_t io_uring_entries = 64;

void io_uring_instance::setup() {
	struct io_uring_params p;
	memset(&p, 0, sizeof(p));

	auto syscall_ret = do_syscall(SYS_io_uring_setup, io_uring_entries, &p);
	__ensure(!sc_error(syscall_ret));
	fd = sc_int_result<int>(syscall_ret);

	// 1. Map Submission Queue SQ and SQEs
	int sq_ring_sz = p.sq_off.array + p.sq_entries * sizeof(unsigned);

	void *sq_ptr = nullptr;
	int ret = sysdep<VmMap>(
	    nullptr,
	    sq_ring_sz,
	    PROT_READ | PROT_WRITE,
	    MAP_SHARED | MAP_POPULATE,
	    fd,
	    IORING_OFF_SQ_RING,
	    &sq_ptr
	);
	__ensure(ret == 0);

	sq.head = reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(sq_ptr) + p.sq_off.head);
	sq.tail = reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(sq_ptr) + p.sq_off.tail);
	sq.ring_mask =
	    reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(sq_ptr) + p.sq_off.ring_mask);
	sq.ring_entries =
	    reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(sq_ptr) + p.sq_off.ring_entries);
	sq.flags = reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(sq_ptr) + p.sq_off.flags);
	sq.array = reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(sq_ptr) + p.sq_off.array);

	// Map the actual SQE array
	size_t sqes_sz = p.sq_entries * sizeof(struct io_uring_sqe);
	ret = sysdep<VmMap>(
	    nullptr,
	    sqes_sz,
	    PROT_READ | PROT_WRITE,
	    MAP_SHARED | MAP_POPULATE,
	    fd,
	    IORING_OFF_SQES,
	    reinterpret_cast<void **>(&sqes)
	);
	__ensure(ret == 0);

	// 2. Map Completion Queue CQ
	size_t cq_ring_sz = p.cq_off.cqes + p.cq_entries * sizeof(struct io_uring_cqe);
	void *cq_ptr = nullptr;
	ret = sysdep<VmMap>(
	    nullptr,
	    cq_ring_sz,
	    PROT_READ | PROT_WRITE,
	    MAP_SHARED | MAP_POPULATE,
	    fd,
	    IORING_OFF_CQ_RING,
	    &cq_ptr
	);
	__ensure(ret == 0);

	cq.head = reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(cq_ptr) + p.cq_off.head);
	cq.tail = reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(cq_ptr) + p.cq_off.tail);
	cq.ring_mask =
	    reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(cq_ptr) + p.cq_off.ring_mask);
	cq.ring_entries =
	    reinterpret_cast<unsigned *>(reinterpret_cast<uintptr_t>(cq_ptr) + p.cq_off.ring_entries);
	cq.cqes = reinterpret_cast<io_uring_cqe *>(reinterpret_cast<uintptr_t>(cq_ptr) + p.cq_off.cqes);
}

std::expected<int, int> io_uring_instance::enter(
    unsigned int to_submit, unsigned int min_complete, unsigned int flags, const sigset_t *sig
) {
	auto ret = do_syscall(SYS_io_uring_enter, fd, to_submit, min_complete, flags, sig, _NSIG / 8);
	if (int e = sc_error(ret); e)
		return std::unexpected{e};
	return sc_int_result<int>(ret);
}

struct AioListioContext {
	// Futex word that doubles as a counter for the remaining in-flight requests.
	int pending;
	// Notification delivery mechanism.
	struct sigevent sevp;
	// Set to `true` if the `AioContext` is responsible for freeing this struct.
	bool ownedByContext;
};

struct AioContext {
	struct aiocb *parent;
	int fildes = -1;
	io_uring_sqe *sqe = nullptr;
	AioListioContext *listioContext = nullptr;
	int *syncResult = nullptr;
	frg::default_list_hook<AioContext> hook_{};
};

using AioRequestQueue = frg::intrusive_list<
    AioContext,
    frg::locate_member<AioContext, frg::default_list_hook<AioContext>, &AioContext::hook_>>;

struct SpawnThreadContext {
	void (*func)(union sigval);
	sigval val;
};

static void *invokeThreadNotification(void *context) {
	auto ctx = static_cast<SpawnThreadContext *>(context);
	ctx->func(ctx->val);
	frg::destruct<SpawnThreadContext>(getAllocator(), ctx);
	return nullptr;
}

static void handleCompletionNotification(struct sigevent *sevp) {
	if (!sevp || sevp->sigev_notify == SIGEV_NONE) {
		return;
	} else if (sevp->sigev_notify == SIGEV_SIGNAL) {
		siginfo_t si{};
		si.si_signo = sevp->sigev_signo;
		si.si_code = SI_ASYNCIO;
		si.si_value = sevp->sigev_value;
		si.si_pid = getpid();
		si.si_uid = getuid();

		do_syscall(SYS_rt_sigqueueinfo, si.si_pid, si.si_signo, &si);
	} else if (sevp->sigev_notify == SIGEV_THREAD) {
		pthread_attr_t attr;
		if (sevp->sigev_notify_attributes)
			attr = *sevp->sigev_notify_attributes;
		else
			pthread_attr_init(&attr);

		int ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		__ensure(!ret);

		pthread_t pthread;
		// We construct the context here, but move ownership to the spawned thread.
		// It is up to the invokeThreadNotification function to destroy the context after use.
		auto threadContext = frg::construct<SpawnThreadContext>(getAllocator());
		threadContext->func = sevp->sigev_notify_function;
		threadContext->val = sevp->sigev_value;
		ret = pthread_create(&pthread, &attr, invokeThreadNotification, threadContext);
		__ensure(!ret);
	} else {
		__ensure(!"unsupported");
	}
}

struct AioWorkerContext {
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

	// new requests waiting to be submitted to the io_uring; protected by lock
	AioRequestQueue queue;

	// in-flight (= submitted to io_uring) requests
	// not protected by the mutex, only accessed by the worker
	AioRequestQueue inFlight;

	// true if the pthread is set up; protected by lock
	bool initialized;
	// worker pthread instance; protected by lock for first-one-wins semantics
	pthread_t pthread;

	// eventfd to ring for newly submitted aiocbs
	int submissionFd;
	// eventfd rung by io_uring
	int ringFd;
	// epoll instance for monitoring the eventfds
	int epollFd;

	// not protected by the mutex, as it is only accessed by the worker
	io_uring_instance ring;

	// Handle all available requests queued for the worker thread.
	void drainQueue();
	// Handle all available CQEs on the io_uring.
	void drainIoUring();
	// Consume a single io_uring CQE; this includes setting the result state to the
	// controlling `struct aiocb`.
	std::pair<struct aiocb *, AioOpState> handle_cqe(io_uring_cqe *cqe);

	// Check the io_uring for available CQEs. The `cqeHandler` is called for every CQE,
	// until either running out of CQEs, or the `cqeHandler` returning true to stop early.
	template <typename F>
	void handle_completions(F cqeHandler) {
		while (true) {
			auto head = __atomic_load_n(ring.cq.head, __ATOMIC_ACQUIRE);
			auto tail = *ring.cq.tail;
			auto mask = *ring.cq.ring_mask;

			if (head == tail)
				break;

			while (head != tail) {
				auto cqe = &ring.cq.cqes[head & mask];
				auto [aiocb, ret] = handle_cqe(cqe);
				head++;

				if (cqeHandler(aiocb, ret)) {
					__atomic_store_n(ring.cq.head, head, __ATOMIC_RELEASE);
					return;
				}
			}

			__atomic_store_n(ring.cq.head, head, __ATOMIC_RELEASE);
		}
	}
} aioWorker;

std::pair<struct aiocb *, AioOpState> AioWorkerContext::handle_cqe(io_uring_cqe *cqe) {
	auto ctx = reinterpret_cast<AioContext *>(cqe->user_data & userDataMask);
	auto result = static_cast<sc_result_t>(cqe->res);
	auto err = sc_error(result);

	if (cqe->user_data & cancelBit) {
		int ret = AIO_NOTCANCELED;

		// io_uring returns ENOENT when the cancellation target was not found, i.e. has already
		// been completed.
		if (err == ENOENT)
			ret = AIO_ALLDONE;
		else if (!err)
			ret = AIO_CANCELED;

		if (ctx->syncResult) {
			__atomic_store_n(ctx->syncResult, ret, __ATOMIC_RELEASE);
			sysdep<FutexWake>(reinterpret_cast<int *>(ctx->syncResult), true);
		}

		if (cqe->user_data & cancelFdBit) {
			int fd = ctx->fildes;

			// Mark every affected in-flight request as cancelled.
			for (auto e : inFlight) {
				if (!e->parent || e->parent->aio_fildes != fd)
					continue;

				AioCbView cbv{e->parent};
				cbv.set_operation_result(std::unexpected{ECANCELED});
				cbv.exchange_state(AioOpState::cancelled, AioOpState::in_progress);
			}
		} else if (ctx->parent) {
			AioCbView cbv{ctx->parent};
			cbv.set_operation_result(std::unexpected{ECANCELED});
			cbv.exchange_state(AioOpState::cancelled, AioOpState::in_progress);
		}

		inFlight.erase(inFlight.iterator_to(ctx));
		frg::destruct(getAllocator(), ctx->sqe);
		frg::destruct(getAllocator(), ctx);
		return {nullptr, AioOpState::invalid};
	} else {
		auto aiocb = ctx->parent;
		AioCbView cbv{aiocb};
		cbv.set_operation_result(
		    err ? std::unexpected{err} : std::expected<ssize_t, int>{sc_int_result<ssize_t>(result)}
		);

		inFlight.erase(inFlight.iterator_to(ctx));
		aiocb->__ctx = nullptr;

		auto listio = ctx->listioContext;
		AioOpState state = (err == ECANCELED) ? AioOpState::cancelled : AioOpState::success;
		cbv.exchange_state(state, AioOpState::in_progress);

		if (listio) {
			int prev = __atomic_fetch_sub(&listio->pending, 1, __ATOMIC_RELEASE);
			if (prev == 1) {
				__atomic_thread_fence(__ATOMIC_ACQUIRE);
				handleCompletionNotification(&listio->sevp);
				sysdep<FutexWake>((int *)&listio->pending, true);
				if (listio->ownedByContext)
					frg::destruct(getAllocator(), listio);
			}
		}

		frg::destruct(getAllocator(), ctx->sqe);
		frg::destruct(getAllocator(), ctx);

		return {aiocb, state};
	}
}

void AioWorkerContext::drainQueue() {
	while (true) {
		AioContext *req = nullptr;

		{
			int ret = pthread_mutex_lock(&lock);
			__ensure(ret == 0);
			frg::scope_exit onExit{[&] { pthread_mutex_unlock(&lock); }};

			if (queue.empty())
				return;

			req = queue.pop_front();
		}

		__ensure(req);
		inFlight.push_back(req);

		auto res = ring.enqueue([&](io_uring_sqe *sqe) { memcpy(sqe, req->sqe, sizeof(*sqe)); });

		__ensure(res.has_value());
	}
}

void AioWorkerContext::drainIoUring() {
	handle_completions([](auto cb, AioOpState) {
		if (cb)
			handleCompletionNotification(&cb->aio_sigevent);
		return false;
	});
}

void *aioWorkerThread(void *) {
	// Block all signals in the worker thread to avoid stealing them from the user threads
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, nullptr);

	struct epoll_event events[2];

	while (true) {
		// poll our two eventfds for being readable
		int nfds = epoll_wait(aioWorker.epollFd, events, 2, -1);
		if (nfds < 0 && errno == EINTR)
			continue;

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == aioWorker.submissionFd) {
				// clear the eventfd first
				uint64_t val;
				read(aioWorker.submissionFd, &val, sizeof(val));

				aioWorker.drainQueue();
			} else if (events[i].data.fd == aioWorker.ringFd) {
				// clear the eventfd first
				uint64_t val;
				read(aioWorker.ringFd, &val, sizeof(val));

				aioWorker.drainIoUring();
			} else {
				auto fd = events[i].data.fd;
				mlibc::infoLogger()
				    << frg::fmt("mlibc: AIO n={} unexpected fd {}", nfds, fd) << frg::endlog;
			}
		}
	}
}

void setupObserverThread() {
	if (__atomic_load_n(&aioWorker.initialized, __ATOMIC_ACQUIRE) == false) {
		int ret = pthread_mutex_lock(&aioWorker.lock);
		__ensure(ret == 0);

		if (__atomic_load_n(&aioWorker.initialized, __ATOMIC_ACQUIRE) == false) {
			// setup worker thread
			aioWorker.submissionFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
			aioWorker.ringFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
			aioWorker.epollFd = epoll_create1(0);
			aioWorker.ring.setup();
			aioWorker.ring.register_eventfd(aioWorker.ringFd);

			struct epoll_event ev{};
			ev.events = EPOLLIN;
			ev.data.fd = aioWorker.submissionFd;
			epoll_ctl(aioWorker.epollFd, EPOLL_CTL_ADD, aioWorker.submissionFd, &ev);
			ev.data.fd = aioWorker.ringFd;
			epoll_ctl(aioWorker.epollFd, EPOLL_CTL_ADD, aioWorker.ringFd, &ev);

			pthread_create(&aioWorker.pthread, nullptr, aioWorkerThread, nullptr);

			__atomic_store_n(&aioWorker.initialized, true, __ATOMIC_RELEASE);
		}

		pthread_mutex_unlock(&aioWorker.lock);
	}
}

void enqueueWorkerRequest(AioContext *req) {
	__ensure(req);

	setupObserverThread();

	{
		int ret = pthread_mutex_lock(&aioWorker.lock);
		__ensure(ret == 0);

		// enqueue
		aioWorker.queue.push_back(req);

		pthread_mutex_unlock(&aioWorker.lock);
	}

	uint64_t plusOne = 1;
	write(aioWorker.submissionFd, &plusOne, sizeof(plusOne));
}

} // namespace

int Sysdeps<AioRead>::operator()(struct aiocb *cb) {
	__ensure((reinterpret_cast<uintptr_t>(cb) & (sizeof(uintptr_t) - 1)) == 0);

	AioCbView cbv{cb};

	auto ctx = frg::construct<AioContext>(getAllocator(), cb);
	ctx->sqe = frg::construct<io_uring_sqe>(getAllocator());
	ctx->sqe->opcode = IORING_OP_READ;
	ctx->sqe->fd = cb->aio_fildes;
	ctx->sqe->addr = reinterpret_cast<uintptr_t>(cb->aio_buf);
	ctx->sqe->len = cb->aio_nbytes;
	ctx->sqe->off = cb->aio_offset;
	ctx->sqe->user_data = reinterpret_cast<uintptr_t>(ctx);
	// TODO: handle ioprio
	cb->__ctx = ctx;
	cbv.reset_operation_result();
	cbv.set_state(AioOpState::in_progress);
	enqueueWorkerRequest(ctx);

	return 0;
}

int Sysdeps<AioWrite>::operator()(struct aiocb *cb) {
	__ensure((reinterpret_cast<uintptr_t>(cb) & (sizeof(uintptr_t) - 1)) == 0);

	AioCbView cbv{cb};

	auto ctx = frg::construct<AioContext>(getAllocator(), cb);
	ctx->sqe = frg::construct<io_uring_sqe>(getAllocator());
	ctx->sqe->opcode = IORING_OP_WRITE;
	ctx->sqe->fd = cb->aio_fildes;
	ctx->sqe->addr = reinterpret_cast<uintptr_t>(cb->aio_buf);
	ctx->sqe->len = cb->aio_nbytes;
	ctx->sqe->off = cb->aio_offset;
	ctx->sqe->user_data = reinterpret_cast<uintptr_t>(ctx);
	// TODO: handle ioprio
	cb->__ctx = ctx;
	cbv.reset_operation_result();
	cbv.set_state(AioOpState::in_progress);
	enqueueWorkerRequest(ctx);

	return 0;
}

namespace {

// Wrapper around the fcntl sysdep to handle the va_list argument.
int fcntl_wrapper(int *result, int fd, int cmd, ...) {
	va_list args;
	va_start(args, cmd);
	int ret = sysdep_or_enosys<Fcntl>(fd, cmd, args, result);
	va_end(args);
	return ret;
}

} // namespace

int Sysdeps<AioFsync>::operator()(int op, struct aiocb *cb) {
	AioCbView cbv{cb};

	int fsync_flags = 0;
	if (op == O_DSYNC)
		fsync_flags = IORING_FSYNC_DATASYNC;
	else if (op == O_SYNC)
		fsync_flags = 0;
	else
		return EINVAL;

	// we must detect EBADF conditions synchronously
	int dummy = 0;
	if (fcntl_wrapper(&dummy, cb->aio_fildes, F_GETFL, 0)) {
		cbv.set_operation_result(std::unexpected{EBADF});
		cbv.set_state(AioOpState::success, true);
		return EBADF;
	}

	__ensure((reinterpret_cast<uintptr_t>(cb) & (sizeof(uintptr_t) - 1)) == 0);

	auto ctx = frg::construct<AioContext>(getAllocator(), cb);
	ctx->sqe = frg::construct<io_uring_sqe>(getAllocator());
	ctx->sqe->opcode = IORING_OP_FSYNC;
	ctx->sqe->fd = cb->aio_fildes;
	ctx->sqe->fsync_flags = fsync_flags;
	ctx->sqe->user_data = reinterpret_cast<uintptr_t>(ctx);
	cb->__ctx = ctx;
	cbv.reset_operation_result();
	cbv.set_state(AioOpState::in_progress);
	enqueueWorkerRequest(ctx);

	return 0;
}

int Sysdeps<AioListio>::operator()(
    int mode, struct aiocb *__restrict const list[], int n, struct sigevent *__restrict sevp
) {
	// Count valid opcodes, skipping over LIO_NOP.
	uint32_t validOpcodes = 0;
	for (int i = 0; i < n; i++) {
		if (list[i]
		    && (list[i]->aio_lio_opcode == LIO_READ || list[i]->aio_lio_opcode == LIO_WRITE))
			validOpcodes++;
	}

	if (!validOpcodes) {
		if (mode == LIO_NOWAIT)
			handleCompletionNotification(sevp);
		return 0;
	}

	AioListioContext *listioContext = nullptr;
	if (mode == LIO_NOWAIT) {
		if (sevp && sevp->sigev_notify != SIGEV_NONE) {
			listioContext = frg::construct<AioListioContext>(getAllocator());
			__atomic_store_n(&listioContext->pending, validOpcodes, __ATOMIC_RELAXED);
			listioContext->sevp = *sevp;
			listioContext->ownedByContext = true;
		}
	} else {
		listioContext = frg::construct<AioListioContext>(getAllocator());
		__atomic_store_n(&listioContext->pending, validOpcodes, __ATOMIC_RELAXED);
		if (sevp)
			listioContext->sevp = *sevp;
		else
			listioContext->sevp.sigev_notify = SIGEV_NONE;
		listioContext->ownedByContext = false;
	}

	for (int i = 0; i < n; i++) {
		struct aiocb *cb = list[i];
		if (!cb || cb->aio_lio_opcode == LIO_NOP)
			continue;

		int opcode = 0;
		if (cb->aio_lio_opcode == LIO_READ)
			opcode = IORING_OP_READ;
		else if (cb->aio_lio_opcode == LIO_WRITE)
			opcode = IORING_OP_WRITE;
		else
			continue;

		if (cb->aio_nbytes > SSIZE_MAX)
			return EINVAL;

		AioCbView cbv{cb};
		auto ctx = frg::construct<AioContext>(getAllocator(), cb);
		ctx->sqe = frg::construct<io_uring_sqe>(getAllocator());
		ctx->sqe->opcode = opcode;
		ctx->sqe->fd = cb->aio_fildes;
		ctx->sqe->addr = reinterpret_cast<uintptr_t>(cb->aio_buf);
		ctx->sqe->len = cb->aio_nbytes;
		ctx->sqe->off = cb->aio_offset;
		ctx->sqe->user_data = reinterpret_cast<uintptr_t>(ctx);
		ctx->listioContext = listioContext;
		cb->__ctx = ctx;
		cbv.reset_operation_result();
		cbv.set_state(AioOpState::in_progress);
		enqueueWorkerRequest(ctx);
	}

	if (mode == LIO_WAIT) {
		// Wait for the `pending` futex word to be zeroed by the completion of the last request
		while (__atomic_load_n(&listioContext->pending, __ATOMIC_ACQUIRE)) {
			int expected = __atomic_load_n(&listioContext->pending, __ATOMIC_RELAXED);
			if (expected == 0)
				break;
			int e = sysdep<FutexWait>((int *)&listioContext->pending, expected, nullptr);
			if (e && e != EAGAIN)
				return e;
		}
		__ensure(!listioContext->ownedByContext);
		frg::destruct(getAllocator(), listioContext);
	}

	return 0;
}

int Sysdeps<AioSuspend>::operator()(
    const struct aiocb *const list[], int n, const struct timespec *ts
) {
	frg::vector<struct futex_waitv, MemoryAllocator> waiters{getAllocator()};

	// Loop over the `list` once. This checks whether we already see completed requests and builds
	// the wait list for `futex_waitv`.
	for (int i = 0; i < n; i++) {
		if (!list[i])
			continue;

		auto state = ConstAioCbView(list[i]).get_state();
		if (state == AioOpState::in_progress) {
			waiters.emplace_back(
			    std::to_underlying(AioOpState::in_progress),
			    reinterpret_cast<uintptr_t>(&list[i]->__state),
			    2,
			    0
			);
		} else if (state == AioOpState::success || state == AioOpState::cancelled) {
			return 0;
		} else {
			mlibc::infoLogger() << frg::fmt(
			    "mlibc: unexpected aiocb status {} in aio_suspend", std::to_underlying(state)
			) << frg::endlog;
			__ensure(!"unimplemented");
		}
	}

	if (waiters.empty())
		return 0;

	// futex_waitv seems to use absolute time, while aio_suspend uses relative
	struct timespec absolute{};
	if (ts)
		if (!time_relative_to_absolute(CLOCK_MONOTONIC, ts, &absolute))
			return EINVAL;

	while (true) {
		auto ret = do_syscall(
		    SYS_futex_waitv,
		    waiters.data(),
		    waiters.size(),
		    0,
		    ts ? &absolute : nullptr,
		    CLOCK_MONOTONIC
		);
		int e = sc_error(ret);
		if (e == ETIMEDOUT)
			// translate ETIMEDOUT to EAGAIN
			return EAGAIN;
		else if (e && e != EAGAIN)
			// return errors other than EAGAIN, which tells us to retry
			return e;

		for (int i = 0; i < n; i++) {
			if (!list[i])
				continue;

			auto state = ConstAioCbView(list[i]).get_state();

			if (state == AioOpState::success || state == AioOpState::cancelled) {
				return 0;
			} else if (state != AioOpState::in_progress) {
				mlibc::infoLogger() << frg::fmt(
				    "mlibc: unexpected aiocb status {} in aio_suspend", std::to_underlying(state)
				) << frg::endlog;
				__ensure(!"unimplemented");
			}
		}
	}
}

int Sysdeps<AioCancel>::operator()(int fildes, struct aiocb *cb, int *result) {
	// we must detect EBADF conditions synchronously
	int dummy = 0;
	if (fcntl_wrapper(&dummy, fildes, F_GETFL, 0))
		return EBADF;

	if (cb) {
		auto state = ConstAioCbView(cb).get_state();
		if (state != AioOpState::in_progress) {
			*result = AIO_ALLDONE;
			return 0;
		}
		if (!cb->__ctx) {
			*result = AIO_NOTCANCELED;
			return 0;
		}
	}

	int syncRes = -1;
	auto ctx = frg::construct<AioContext>(getAllocator(), cb);
	ctx->sqe = frg::construct<io_uring_sqe>(getAllocator());
	ctx->sqe->opcode = IORING_OP_ASYNC_CANCEL;
	ctx->syncResult = &syncRes;

	if (!cb) {
		ctx->fildes = fildes;
		ctx->sqe->fd = fildes;
		ctx->sqe->user_data = reinterpret_cast<uintptr_t>(ctx) | cancelBit | cancelFdBit;
		ctx->sqe->cancel_flags = IORING_ASYNC_CANCEL_ALL | IORING_ASYNC_CANCEL_FD;
	} else {
		ctx->sqe->addr = reinterpret_cast<uintptr_t>(cb->__ctx);
		ctx->sqe->user_data = reinterpret_cast<uintptr_t>(ctx) | cancelBit;
	}

	enqueueWorkerRequest(ctx);

	while (__atomic_load_n(&syncRes, __ATOMIC_ACQUIRE) == -1)
		sysdep<FutexWait>((int *)&syncRes, -1, nullptr);

	*result = __atomic_load_n(&syncRes, __ATOMIC_RELAXED);
	return 0;
}

} // namespace mlibc

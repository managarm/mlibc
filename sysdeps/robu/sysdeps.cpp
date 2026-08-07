#include <errno.h>
#include <string.h>
#include <mlibc/all-sysdeps.hpp>
#include <bits/ensure.h>

#include "robu-abi.hpp"

extern "C" uint64_t __robu_heap_base;

namespace {

int64_t g_console_handle = -1;

int64_t console_handle() {
	if (g_console_handle < 0) {
		g_console_handle = robu::devfs_open("/dev/console");
	}
	return g_console_handle;
}

void console_write_all(const char *buf, size_t len) {
	int64_t h = console_handle();
	if (h < 0) {
		return;
	}
	size_t off = 0;
	while (off < len) {
		size_t chunk = len - off;
		if (chunk > (size_t)robu::DEVFS_WRITE_MAX) {
			chunk = robu::DEVFS_WRITE_MAX;
		}
		robu::devfs_write((uint64_t)h, buf + off, chunk);
		off += chunk;
	}
}

uint8_t *g_anon_cursor;

}

namespace mlibc {

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	console_write_all(msg, strlen(msg));
	console_write_all("\n", 1);
}

int Sysdeps<Isatty>::operator()(int fd) {
	if (fd == 0 || fd == 1 || fd == 2) {
		return 0;
	}
	return ENOTTY;
}

int Sysdeps<Write>::operator()(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	if (fd != 1 && fd != 2) {
		return EBADF;
	}
	console_write_all((const char *)buf, count);
	*bytes_written = (ssize_t)count;
	return 0;
}

int Sysdeps<Read>::operator()(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	if (fd != 0) {
		return EBADF;
	}
	int64_t h = console_handle();
	if (h < 0) {
		return EIO;
	}
	if (count > (size_t)robu::DEVFS_READ_MAX) {
		count = robu::DEVFS_READ_MAX;
	}
	int64_t n = robu::devfs_read((uint64_t)h, buf, count);
	if (n < 0) {
		return EIO;
	}
	*bytes_read = (ssize_t)n;
	return 0;
}

int Sysdeps<Open>::operator()(const char *, int, mode_t, int *) {
	return ENOSYS;
}

int Sysdeps<Close>::operator()(int fd) {
	if (fd == 0 || fd == 1 || fd == 2) {
		return 0;
	}
	return EBADF;
}

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	return ESPIPE;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	robu::msg_regs m{};
	m.word[0] = (uint64_t)pointer;
	int64_t rc = robu::ipc_raw(0, 0, robu::IPC_FLAG_SET_FSBASE, &m, nullptr);
	return rc == robu::IPC_ERR_NONE ? 0 : EINVAL;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	if (!g_anon_cursor) {
		g_anon_cursor = (uint8_t *)__robu_heap_base;
	}
	size = (size + 0xFFF) & ~(size_t)0xFFF;
	*pointer = g_anon_cursor;
	g_anon_cursor += size;
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *, size_t) {
	return 0;
}

int Sysdeps<VmMap>::operator()(void *, size_t size, int, int, int fd, off_t, void **window) {
	if (fd != -1) {
		return ENOSYS;
	}
	void *p;
	Sysdeps<AnonAllocate>::operator()(size, &p);
	*window = p;
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	return 0;
}

int Sysdeps<FutexWait>::operator()(int *, int, const struct timespec *) {
	return ENOSYS;
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	return ENOSYS;
}

int Sysdeps<ClockGet>::operator()(int, time_t *secs, long *nanos) {
	uint64_t ticks = robu::kinfo_ticks();
	uint64_t hz = robu::kinfo()->clock_hz ? robu::kinfo()->clock_hz : 1;
	*secs = (time_t)(ticks / hz);
	*nanos = (long)((ticks % hz) * (1000000000ull / hz));
	return 0;
}

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	robu::exit_raw(status);
}

}

#pragma once

#include <stdint.h>

namespace robu {

struct msg_regs {
	uint64_t word[6];
};

constexpr uint64_t IPC_TID_ANY = 0;
constexpr uint64_t IPC_FLAG_NONE = 0;
constexpr uint64_t IPC_FLAG_NOBLOCK = 1ull << 4;
constexpr uint64_t IPC_FLAG_RECV = 1ull << 1;
constexpr uint64_t IPC_FLAG_CONSOLE_WRITE = 1ull << 7;
constexpr uint64_t IPC_FLAG_EXIT = 1ull << 14;
constexpr uint64_t IPC_FLAG_SPAWN = 1ull << 15;
constexpr uint64_t IPC_FLAG_WAIT = 1ull << 16;
constexpr uint64_t IPC_FLAG_CONSOLE_READ = 1ull << 17;
constexpr uint64_t IPC_FLAG_SET_FSBASE = 1ull << 25;

constexpr int64_t IPC_ERR_NONE = 0;
constexpr int64_t IPC_ERR_NOT_FOUND = -1;
constexpr int64_t IPC_ERR_WOULDBLOCK = -5;
constexpr int64_t IPC_ERR_NO_MEM = -6;

inline int64_t ipc_raw(uint64_t dest, uint64_t src_or_arg, uint64_t flags,
                        msg_regs *io, uint32_t *from_out) {
	register uint64_t r8 asm("r8") = io ? io->word[0] : 0;
	register uint64_t r9 asm("r9") = io ? io->word[1] : 0;
	register uint64_t r10 asm("r10") = io ? io->word[2] : 0;
	register uint64_t r12 asm("r12") = io ? io->word[3] : 0;
	register uint64_t r13 asm("r13") = io ? io->word[4] : 0;
	register uint64_t r14 asm("r14") = io ? io->word[5] : 0;
	register uint64_t rdi asm("rdi") = dest;
	register uint64_t rsi asm("rsi") = src_or_arg;
	register uint64_t rdx asm("rdx") = flags;
	int64_t status;
	asm volatile("int $0x30"
	             : "=a"(status), "+r"(rsi),
	               "+r"(r8), "+r"(r9), "+r"(r10),
	               "+r"(r12), "+r"(r13), "+r"(r14)
	             : "r"(rdi), "r"(rdx)
	             : "r11", "rcx", "memory", "cc");
	if (io) {
		io->word[0] = r8;
		io->word[1] = r9;
		io->word[2] = r10;
		io->word[3] = r12;
		io->word[4] = r13;
		io->word[5] = r14;
	}
	if (from_out) {
		*from_out = (uint32_t)rsi;
	}
	return status;
}

inline int64_t ipc_call(uint32_t dest, msg_regs *io, uint32_t *from) {
	return ipc_raw(dest, dest, IPC_FLAG_RECV, io, from);
}

[[noreturn]] inline void exit_raw(int status) {
	msg_regs m{};
	m.word[0] = (uint64_t)(uint8_t)status;
	ipc_raw(0, 0, IPC_FLAG_EXIT, &m, nullptr);
	for (;;) {
	}
}

constexpr uint64_t DEVFS_OP_OPEN = 1;
constexpr uint64_t DEVFS_OP_READ = 2;
constexpr uint64_t DEVFS_OP_WRITE = 3;
constexpr int DEVFS_PATH_MAX = 40;
constexpr int DEVFS_READ_MAX = 40;
constexpr int DEVFS_WRITE_MAX = 24;

constexpr uint64_t KINFO_VA = 0x0000000080000000ULL;

struct kinfo_page {
	uint32_t abi_version_major;
	uint32_t abi_version_minor;
	uint64_t feature_bits;
	uint32_t cpu_count;
	uint32_t boot_apic_id;
	volatile uint32_t clock_seq;
	uint64_t clock_ticks;
	uint32_t clock_hz;
	uint32_t devfs_tid;
	uint32_t test_report_tid;
	uint32_t benchserver_tid;
	uint32_t abitest_helper_tid;
	uint32_t abitest_slots[5];
	uint32_t ramfs_tid;
	uint32_t abitest_exit_helper_tid;
	uint32_t procfs_tid;
	uint32_t sysfs_tid;
};

inline const kinfo_page *kinfo() {
	return reinterpret_cast<const kinfo_page *>(KINFO_VA);
}

inline uint64_t kinfo_ticks() {
	const kinfo_page *k = kinfo();
	uint32_t seq0, seq1;
	uint64_t ticks;
	do {
		seq0 = k->clock_seq;
		asm volatile("" ::: "memory");
		ticks = k->clock_ticks;
		asm volatile("" ::: "memory");
		seq1 = k->clock_seq;
	} while (seq0 != seq1 || (seq0 & 1u));
	return ticks;
}

inline uint32_t devfs_tid() {
	return kinfo()->devfs_tid;
}

inline int64_t devfs_open(const char *path) {
	msg_regs m{};
	m.word[0] = DEVFS_OP_OPEN;
	char *dst = reinterpret_cast<char *>(&m.word[1]);
	int i = 0;
	for (; path[i] && i < DEVFS_PATH_MAX - 1; i++) {
		dst[i] = path[i];
	}
	dst[i] = '\0';
	uint32_t from;
	ipc_call(devfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	uint64_t handle = m.word[1];
	return status == 0 ? (int64_t)handle : status;
}

inline int64_t devfs_read(uint64_t handle, void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = DEVFS_OP_READ;
	m.word[1] = handle;
	m.word[2] = len > (uint64_t)DEVFS_READ_MAX ? (uint64_t)DEVFS_READ_MAX : len;
	uint32_t from;
	ipc_call(devfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status > 0) {
		const uint8_t *data = reinterpret_cast<const uint8_t *>(&m.word[1]);
		uint8_t *out = reinterpret_cast<uint8_t *>(buf);
		for (int64_t i = 0; i < status; i++) {
			out[i] = data[i];
		}
	}
	return status;
}

inline int64_t devfs_write(uint64_t handle, const void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = DEVFS_OP_WRITE;
	m.word[1] = handle;
	uint64_t clamped = len > (uint64_t)DEVFS_WRITE_MAX ? (uint64_t)DEVFS_WRITE_MAX : len;
	m.word[2] = clamped;
	uint8_t *dst = reinterpret_cast<uint8_t *>(&m.word[3]);
	const uint8_t *src = reinterpret_cast<const uint8_t *>(buf);
	for (uint64_t i = 0; i < clamped; i++) {
		dst[i] = src[i];
	}
	uint32_t from;
	ipc_call(devfs_tid(), &m, &from);
	return (int64_t)m.word[0];
}

constexpr uint64_t DEVFS_OP_CLOSE = 4;

inline int64_t devfs_close(uint64_t handle) {
	msg_regs m{};
	m.word[0] = DEVFS_OP_CLOSE;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(devfs_tid(), &m, &from);
	return (int64_t)m.word[0];
}

// --- ramfs -----------------------------------------------------------------

constexpr uint64_t RAMFS_OP_OPEN    = 1;
constexpr uint64_t RAMFS_OP_READ    = 2;
constexpr uint64_t RAMFS_OP_WRITE   = 3;
constexpr uint64_t RAMFS_OP_CLOSE   = 4;
constexpr uint64_t RAMFS_OP_STAT    = 5;
constexpr uint64_t RAMFS_OP_FSTAT   = 6;
constexpr uint64_t RAMFS_OP_READDIR = 7;
constexpr int RAMFS_NAME_MAX  = 20;
constexpr int RAMFS_PATH_MAX  = 32;
constexpr int RAMFS_READ_MAX  = 40;
constexpr int RAMFS_WRITE_MAX = 24;
constexpr uint64_t RAMFS_ROOT_INO = 1;
constexpr uint64_t RAMFS_O_CREAT  = 0x0040;
constexpr uint64_t RAMFS_O_TRUNC  = 0x0200;
constexpr uint64_t RAMFS_O_APPEND = 0x0400;

inline uint32_t ramfs_tid() {
	return kinfo()->ramfs_tid;
}

inline void msg_put_str(msg_regs &m, unsigned word_off, const char *s, int max) {
	char *dst = reinterpret_cast<char *>(&m.word[word_off]);
	int i = 0;
	for (; s[i] && i < max - 1; i++) {
		dst[i] = s[i];
	}
	dst[i] = '\0';
}

inline int64_t ramfs_open(const char *name, uint64_t flags) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_OPEN;
	m.word[1] = flags;
	msg_put_str(m, 2, name, RAMFS_PATH_MAX);
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	return status == 0 ? (int64_t)m.word[1] : status;
}

inline int64_t ramfs_read(uint64_t handle, void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_READ;
	m.word[1] = handle;
	m.word[2] = len > (uint64_t)RAMFS_READ_MAX ? (uint64_t)RAMFS_READ_MAX : len;
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status > 0) {
		const uint8_t *data = reinterpret_cast<const uint8_t *>(&m.word[1]);
		uint8_t *out = reinterpret_cast<uint8_t *>(buf);
		for (int64_t i = 0; i < status; i++) {
			out[i] = data[i];
		}
	}
	return status;
}

inline int64_t ramfs_write(uint64_t handle, const void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_WRITE;
	m.word[1] = handle;
	uint64_t clamped = len > (uint64_t)RAMFS_WRITE_MAX ? (uint64_t)RAMFS_WRITE_MAX : len;
	m.word[2] = clamped;
	uint8_t *dst = reinterpret_cast<uint8_t *>(&m.word[3]);
	const uint8_t *src = reinterpret_cast<const uint8_t *>(buf);
	for (uint64_t i = 0; i < clamped; i++) {
		dst[i] = src[i];
	}
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t ramfs_close(uint64_t handle) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_CLOSE;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t ramfs_stat(const char *name, uint64_t *size_out, int *is_dir_out, uint64_t *ino_out) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_STAT;
	msg_put_str(m, 1, name, RAMFS_PATH_MAX);
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0) {
		if (size_out) *size_out = m.word[1];
		if (is_dir_out) *is_dir_out = (int)m.word[2];
		if (ino_out) *ino_out = m.word[3];
	}
	return status;
}

inline int64_t ramfs_fstat(uint64_t handle, uint64_t *size_out, int *is_dir_out, uint64_t *ino_out) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_FSTAT;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0) {
		if (size_out) *size_out = m.word[1];
		if (is_dir_out) *is_dir_out = (int)m.word[2];
		if (ino_out) *ino_out = m.word[3];
	}
	return status;
}

inline int64_t ramfs_readdir(uint64_t dir_ino, uint64_t index, char *name_out, int *is_dir_out) {
	msg_regs m{};
	m.word[0] = RAMFS_OP_READDIR;
	m.word[1] = dir_ino;
	m.word[2] = index;
	uint32_t from;
	ipc_call(ramfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0) {
		if (is_dir_out) *is_dir_out = (int)m.word[1];
		const char *src = reinterpret_cast<const char *>(&m.word[2]);
		int i = 0;
		for (; i < RAMFS_NAME_MAX - 1 && src[i]; i++) {
			name_out[i] = src[i];
		}
		name_out[i] = '\0';
	}
	return status;
}

// --- procfs / sysfs (read-only, open/read/close) ----------------------------

constexpr uint64_t PROCFS_OP_OPEN  = 1;
constexpr uint64_t PROCFS_OP_READ  = 2;
constexpr uint64_t PROCFS_OP_CLOSE = 3;
constexpr int PROCFS_PATH_MAX = 24;
constexpr int PROCFS_READ_MAX = 40;

inline uint32_t procfs_tid() {
	return kinfo()->procfs_tid;
}

inline int64_t procfs_open(const char *path, uint64_t *size_out) {
	msg_regs m{};
	m.word[0] = PROCFS_OP_OPEN;
	msg_put_str(m, 1, path, PROCFS_PATH_MAX);
	uint32_t from;
	ipc_call(procfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0 && size_out) {
		*size_out = m.word[2];
	}
	return status == 0 ? (int64_t)m.word[1] : status;
}

inline int64_t procfs_read(uint64_t handle, void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = PROCFS_OP_READ;
	m.word[1] = handle;
	m.word[2] = len > (uint64_t)PROCFS_READ_MAX ? (uint64_t)PROCFS_READ_MAX : len;
	uint32_t from;
	ipc_call(procfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status > 0) {
		const uint8_t *data = reinterpret_cast<const uint8_t *>(&m.word[1]);
		uint8_t *out = reinterpret_cast<uint8_t *>(buf);
		for (int64_t i = 0; i < status; i++) {
			out[i] = data[i];
		}
	}
	return status;
}

inline int64_t procfs_close(uint64_t handle) {
	msg_regs m{};
	m.word[0] = PROCFS_OP_CLOSE;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(procfs_tid(), &m, &from);
	return (int64_t)m.word[0];
}

constexpr uint64_t SYSFS_OP_OPEN  = 1;
constexpr uint64_t SYSFS_OP_READ  = 2;
constexpr uint64_t SYSFS_OP_CLOSE = 3;
constexpr int SYSFS_PATH_MAX = 16;
constexpr int SYSFS_READ_MAX = 40;

inline uint32_t sysfs_tid() {
	return kinfo()->sysfs_tid;
}

inline int64_t sysfs_open(const char *path, uint64_t *size_out) {
	msg_regs m{};
	m.word[0] = SYSFS_OP_OPEN;
	msg_put_str(m, 1, path, SYSFS_PATH_MAX);
	uint32_t from;
	ipc_call(sysfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0 && size_out) {
		*size_out = m.word[2];
	}
	return status == 0 ? (int64_t)m.word[1] : status;
}

inline int64_t sysfs_read(uint64_t handle, void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = SYSFS_OP_READ;
	m.word[1] = handle;
	m.word[2] = len > (uint64_t)SYSFS_READ_MAX ? (uint64_t)SYSFS_READ_MAX : len;
	uint32_t from;
	ipc_call(sysfs_tid(), &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status > 0) {
		const uint8_t *data = reinterpret_cast<const uint8_t *>(&m.word[1]);
		uint8_t *out = reinterpret_cast<uint8_t *>(buf);
		for (int64_t i = 0; i < status; i++) {
			out[i] = data[i];
		}
	}
	return status;
}

inline int64_t sysfs_close(uint64_t handle) {
	msg_regs m{};
	m.word[0] = SYSFS_OP_CLOSE;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(sysfs_tid(), &m, &from);
	return (int64_t)m.word[0];
}

inline uint64_t strlen_(const char *s) {
	uint64_t n = 0;
	while (s[n]) n++;
	return n;
}

// --- spawn / wait ------------------------------------------------------------
// Ungated dest==0 kernel verbs -- not part of POSIX fork()/execve(), this
// kernel has no in-place image-replace primitive. Mirrors the wire format
// apps/libc/src/spawn.c already uses for __libc_spawn()/waitpid().

constexpr uint32_t SPAWN_REQ_MAGIC = 0x314e5053u;
constexpr int SPAWN_MAX_ARGS = 64;
constexpr int SPAWN_REQ_MAX_LEN = 4096;

struct robu_spawn_str {
	uint32_t off;
	uint32_t len;
};

struct robu_spawn_req {
	uint32_t magic;
	uint32_t total_len;
	uint32_t name_off, name_len;
	uint32_t argc, argv_off;
	uint32_t envc, envp_off;
	uint32_t nfds, fds_off;
};

struct robu_spawn_fd {
	uint32_t fd;
	uint32_t kind;
	uint64_t handle;
};

constexpr int SPAWN_FD_INFO_MAX = 3;
constexpr uint32_t SPAWN_INFO_MAGIC = 0x314e4953u;

// Layout of the fd-info block a spawned child finds via its 5th entry arg
// (spawn_info VA): a robu_spawn_info header immediately followed by
// info.nfds robu_spawn_fd entries.
struct robu_spawn_info {
	uint32_t magic;
	uint32_t nfds;
};

// fd_kind/fd_handle: caller-supplied lookup for fds 0/1/2, same shape as
// apps/libc's __libc_fd_export -- return false if that fd isn't open.
inline int64_t robu_spawn(const char *name, char *const argv[], char *const envp[],
                           bool (*fd_export)(int fd, uint32_t *kind, uint64_t *handle)) {
	static uint8_t buf[SPAWN_REQ_MAX_LEN];
	robu_spawn_req *req = reinterpret_cast<robu_spawn_req *>(buf);
	int argc = 0;
	while (argv && argv[argc]) argc++;
	int envc = 0;
	while (envp && envp[envc]) envc++;
	if (argc > SPAWN_MAX_ARGS || envc > SPAWN_MAX_ARGS) {
		return -1;
	}
	robu_spawn_str argv_table[SPAWN_MAX_ARGS];
	robu_spawn_str envp_table[SPAWN_MAX_ARGS];
	uint32_t off = sizeof(*req);
	auto put = [&](const char *s, uint32_t len) -> uint32_t {
		uint32_t o = off;
		for (uint32_t i = 0; i < len; i++) {
			buf[off + i] = (uint8_t)s[i];
		}
		off += len;
		return o;
	};
	if (off + strlen_(name) > sizeof(buf)) return -1;
	uint32_t name_len = (uint32_t)strlen_(name);
	uint32_t name_off = put(name, name_len);
	for (int i = 0; i < argc; i++) {
		uint32_t len = (uint32_t)strlen_(argv[i]);
		if (off + len > sizeof(buf)) return -1;
		argv_table[i].off = put(argv[i], len);
		argv_table[i].len = len;
	}
	for (int i = 0; i < envc; i++) {
		uint32_t len = (uint32_t)strlen_(envp[i]);
		if (off + len > sizeof(buf)) return -1;
		envp_table[i].off = put(envp[i], len);
		envp_table[i].len = len;
	}
	uint32_t argv_off = off;
	if (off + (uint32_t)argc * sizeof(robu_spawn_str) > sizeof(buf)) return -1;
	for (int i = 0; i < argc; i++) {
		reinterpret_cast<robu_spawn_str *>(buf + off)[i] = argv_table[i];
	}
	off += (uint32_t)argc * sizeof(robu_spawn_str);
	uint32_t envp_off = off;
	if (off + (uint32_t)envc * sizeof(robu_spawn_str) > sizeof(buf)) return -1;
	for (int i = 0; i < envc; i++) {
		reinterpret_cast<robu_spawn_str *>(buf + off)[i] = envp_table[i];
	}
	off += (uint32_t)envc * sizeof(robu_spawn_str);
	uint32_t nfds = 0;
	robu_spawn_fd fds[SPAWN_FD_INFO_MAX];
	if (fd_export) {
		for (int fd = 0; fd <= 2; fd++) {
			uint32_t kind;
			uint64_t handle;
			if (fd_export(fd, &kind, &handle)) {
				fds[nfds].fd = (uint32_t)fd;
				fds[nfds].kind = kind;
				fds[nfds].handle = handle;
				nfds++;
			}
		}
	}
	uint32_t fds_off = off;
	if (nfds > 0) {
		if (off + nfds * sizeof(robu_spawn_fd) > sizeof(buf)) return -1;
		for (uint32_t i = 0; i < nfds; i++) {
			reinterpret_cast<robu_spawn_fd *>(buf + off)[i] = fds[i];
		}
		off += nfds * sizeof(robu_spawn_fd);
	}
	req->magic = SPAWN_REQ_MAGIC;
	req->total_len = off;
	req->name_off = name_off;
	req->name_len = name_len;
	req->argc = (uint32_t)argc;
	req->argv_off = argc ? argv_off : 0;
	req->envc = (uint32_t)envc;
	req->envp_off = envc ? envp_off : 0;
	req->nfds = nfds;
	req->fds_off = nfds ? fds_off : 0;
	msg_regs m{};
	m.word[0] = (uint64_t)buf;
	m.word[1] = off;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_SPAWN, &m, nullptr);
	if (rc != IPC_ERR_NONE) {
		return -1;
	}
	return (int64_t)m.word[0];
}

inline int64_t robu_waitpid(int64_t pid, int *status, bool nohang) {
	uint64_t filter = pid > 0 ? (uint64_t)pid : 0;
	uint64_t flags = IPC_FLAG_WAIT;
	if (nohang) {
		flags |= IPC_FLAG_NOBLOCK;
	}
	msg_regs m{};
	m.word[0] = filter;
	int64_t rc = ipc_raw(0, 0, flags, &m, nullptr);
	if (rc == IPC_ERR_WOULDBLOCK) {
		return 0;
	}
	if (rc != IPC_ERR_NONE) {
		return -1;
	}
	if (status) {
		*status = ((int)m.word[1] & 0xff) << 8;
	}
	return (int64_t)m.word[0];
}

}

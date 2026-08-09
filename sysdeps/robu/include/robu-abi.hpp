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
// Matches apps/devfs/devfs.c's local dev_id_t enum -- devfs's console read
// is deliberately non-blocking, so a 0-byte reply means "nothing typed
// yet," not EOF.
constexpr uint64_t DEV_CONSOLE = 0;
constexpr uint64_t IPC_FLAG_EXIT = 1ull << 14;
constexpr uint64_t IPC_FLAG_SPAWN = 1ull << 15;
constexpr uint64_t IPC_FLAG_WAIT = 1ull << 16;
constexpr uint64_t IPC_FLAG_CONSOLE_READ = 1ull << 17;
constexpr uint64_t IPC_FLAG_PIPE_CREATE = 1ull << 18;
constexpr uint64_t IPC_FLAG_PIPE_READ = 1ull << 19;
constexpr uint64_t IPC_FLAG_PIPE_WRITE = 1ull << 20;
constexpr uint64_t IPC_FLAG_PIPE_CLOSE = 1ull << 21;
constexpr uint64_t IPC_FLAG_FORK = 1ull << 24;
constexpr uint64_t IPC_FLAG_SET_FSBASE = 1ull << 25;
constexpr uint64_t IPC_FLAG_SELF_TID = 1ull << 26;
constexpr uint64_t IPC_FLAG_EXEC = 1ull << 27;

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

// --- pipes ---------------------------------------------------------------
// Kernel-resident ring buffers, matching include/robu/pipe.h -- no server
// process involved, these are direct dest==0 null-IPC calls resolved
// entirely in-kernel. Reads/writes are deliberately non-blocking in the
// kernel (0 bytes with IPC_ERR_NONE means "try again", IPC_ERR_NOT_FOUND
// means real EOF/EPIPE) -- callers that want blocking semantics sleep-retry
// on their own, same shape as the console read path.
constexpr uint64_t PIPE_CHUNK_MAX = 32;

inline int64_t pipe_create(uint64_t *out_handle) {
	msg_regs m{};
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_PIPE_CREATE, &m, nullptr);
	*out_handle = m.word[0];
	return rc;
}

inline int64_t pipe_read_raw(uint64_t handle, void *buf, uint64_t max, uint64_t *out_len) {
	msg_regs m{};
	m.word[0] = handle;
	m.word[1] = max > PIPE_CHUNK_MAX ? PIPE_CHUNK_MAX : max;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_PIPE_READ, &m, nullptr);
	*out_len = m.word[0];
	uint64_t n = *out_len > PIPE_CHUNK_MAX ? PIPE_CHUNK_MAX : *out_len;
	uint8_t *dst = (uint8_t *)buf;
	const uint8_t *src = (const uint8_t *)&m.word[2];
	for (uint64_t i = 0; i < n; i++) dst[i] = src[i];
	return rc;
}

inline int64_t pipe_write_raw(uint64_t handle, const void *buf, uint64_t len, uint64_t *out_len) {
	msg_regs m{};
	m.word[0] = handle;
	uint64_t n = len > PIPE_CHUNK_MAX ? PIPE_CHUNK_MAX : len;
	m.word[1] = n;
	uint8_t *dst = (uint8_t *)&m.word[2];
	const uint8_t *src = (const uint8_t *)buf;
	for (uint64_t i = 0; i < n; i++) dst[i] = src[i];
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_PIPE_WRITE, &m, nullptr);
	*out_len = m.word[0];
	return rc;
}

inline int64_t pipe_close_raw(uint64_t handle, int is_write_end) {
	msg_regs m{};
	m.word[0] = handle;
	m.word[1] = (uint64_t)is_write_end;
	return ipc_raw(0, 0, IPC_FLAG_PIPE_CLOSE, &m, nullptr);
}

// --- fork ------------------------------------------------------------------
// IPC_FLAG_FORK is the one call in this whole ABI that returns twice: the
// kernel clones the caller's address space (real, eager, no COW) and
// creates a second thread whose saved register frame is a byte-for-byte
// copy of the caller's -- so *both* threads resume here. `rc` (rax) is
// IPC_ERR_NONE in both threads (the call itself succeeded); word[0] (r8) is
// what actually distinguishes them -- 0 in the child, the real child tid in
// the parent (this is exactly what the removed apps/libc's own fork()
// relied on: `rc = robu_ipc_raw(...); return (pid_t)m.word[0];`). Nothing
// else needs to be built in userspace to make fork() "work" the way it does
// on a fork-based OS: the fd table, heap, and every other bit of process
// state the caller had is already correct in both copies, since it's just
// process memory that got duplicated along with everything else.
inline int64_t fork_raw(uint64_t *out_child_or_zero) {
	msg_regs m{};
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_FORK, &m, nullptr);
	*out_child_or_zero = m.word[0];
	return rc;
}

// No prior primitive told a thread its own tid (spawn/fork replies only
// ever tell the *caller* the *child's* tid) -- needed so a forked child can
// correct its own cached tid (mlibc's FutexTid sysdep) once it starts
// running with a copy of the parent's, which is now wrong for it.
inline uint64_t self_tid() {
	msg_regs m{};
	ipc_raw(0, 0, IPC_FLAG_SELF_TID, &m, nullptr);
	return m.word[0];
}

constexpr uint64_t KINFO_VA = 0x0000000080000000ULL;

// Must stay byte-for-byte in sync with include/robu/kinfo.h's kinfo_page_t
// and mount_entry_t -- this file hand-mirrors the C header instead of
// including it, so any field added on the kernel side needs the matching
// edit here too.
constexpr int MOUNT_PREFIX_MAX = 24;
constexpr int MOUNT_TABLE_MAX = 8;
struct mount_entry {
	uint32_t in_use;
	uint32_t owner_tid;
	char prefix[MOUNT_PREFIX_MAX];
};
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
	mount_entry mounts[MOUNT_TABLE_MAX];
	volatile uint32_t mount_seq;
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

// Longest-prefix match, mirrors include/robu/kinfo.h's kinfo_resolve_mount()
// exactly. Returns 0 (never a real tid) if nothing matches; *matched_len_out
// (if non-null) gets how many leading bytes of `path` matched, so the
// caller can strip them before sending a mount-relative path to the server
// (every migrated server receives paths relative to its own mount point).
inline uint32_t resolve_mount(const char *path, int *matched_len_out = nullptr) {
	const kinfo_page *k = kinfo();
	uint32_t seq0, seq1;
	uint32_t best_tid;
	int best_len;
	do {
		best_tid = 0;
		best_len = -1;
		seq0 = k->mount_seq;
		asm volatile("" ::: "memory");
		for (int i = 0; i < MOUNT_TABLE_MAX; i++) {
			if (!k->mounts[i].in_use) continue;
			int len = 0;
			while (len < MOUNT_PREFIX_MAX && k->mounts[i].prefix[len]) len++;
			int j = 0;
			for (; j < len; j++) {
				if (path[j] != k->mounts[i].prefix[j]) break;
			}
			if (j == len && len > best_len) {
				best_len = len;
				best_tid = k->mounts[i].owner_tid;
			}
		}
		asm volatile("" ::: "memory");
		seq1 = k->mount_seq;
	} while (seq0 != seq1 || (seq0 & 1u));
	if (matched_len_out) {
		*matched_len_out = best_len < 0 ? 0 : best_len;
	}
	return best_tid;
}

inline uint32_t devfs_tid() {
	return kinfo()->devfs_tid;
}

inline void msg_put_str(msg_regs &m, unsigned word_off, const char *s, int max) {
	char *dst = reinterpret_cast<char *>(&m.word[word_off]);
	int i = 0;
	for (; s[i] && i < max - 1; i++) {
		dst[i] = s[i];
	}
	dst[i] = '\0';
}

// --- generic vfs (include/robu/vfs.h) --------------------------------------
// The mount-table-routed protocol every migrated server (devfs/procfs/
// sysfs/ramfs, in that order) speaks. Op codes and wire layout are
// identical to ramfs's own protocol below on purpose -- ramfs.h was the
// richest of the four original bespoke protocols and vfs.h formalizes its
// shape as the shared one. Every function takes an explicit `tid_t server`
// (resolved dynamically via resolve_mount()) instead of hardcoding one
// server's tid the way ramfs_open()/etc. below still do.

constexpr uint64_t VFS_OP_OPEN    = 1;
constexpr uint64_t VFS_OP_READ    = 2;
constexpr uint64_t VFS_OP_WRITE   = 3;
constexpr uint64_t VFS_OP_CLOSE   = 4;
constexpr uint64_t VFS_OP_STAT    = 5;
constexpr uint64_t VFS_OP_FSTAT   = 6;
constexpr uint64_t VFS_OP_READDIR = 7;
constexpr uint64_t VFS_OP_RENAME  = 8;
constexpr uint64_t VFS_OP_UNLINK  = 9;
constexpr int64_t VFS_ERR_NOT_FOUND     = -1;
constexpr int64_t VFS_ERR_NOT_SUPPORTED = -3;
constexpr int VFS_NAME_MAX  = 20;
constexpr int VFS_PATH_MAX  = 32;
constexpr int VFS_READ_MAX  = 40;
constexpr int VFS_WRITE_MAX = 24;
constexpr uint64_t VFS_ROOT_INO = 1;
constexpr uint64_t VFS_O_CREAT  = 0x0040;
constexpr uint64_t VFS_O_TRUNC  = 0x0200;
constexpr uint64_t VFS_O_APPEND = 0x0400;

inline int64_t vfs_open(uint32_t server, const char *path, uint64_t flags) {
	msg_regs m{};
	m.word[0] = VFS_OP_OPEN;
	m.word[1] = flags;
	msg_put_str(m, 2, path, VFS_PATH_MAX);
	uint32_t from;
	ipc_call(server, &m, &from);
	int64_t status = (int64_t)m.word[0];
	return status == 0 ? (int64_t)m.word[1] : status;
}

inline int64_t vfs_read(uint32_t server, uint64_t handle, void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = VFS_OP_READ;
	m.word[1] = handle;
	m.word[2] = len > (uint64_t)VFS_READ_MAX ? (uint64_t)VFS_READ_MAX : len;
	uint32_t from;
	ipc_call(server, &m, &from);
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

inline int64_t vfs_write(uint32_t server, uint64_t handle, const void *buf, uint64_t len) {
	msg_regs m{};
	m.word[0] = VFS_OP_WRITE;
	m.word[1] = handle;
	uint64_t clamped = len > (uint64_t)VFS_WRITE_MAX ? (uint64_t)VFS_WRITE_MAX : len;
	m.word[2] = clamped;
	uint8_t *dst = reinterpret_cast<uint8_t *>(&m.word[3]);
	const uint8_t *src = reinterpret_cast<const uint8_t *>(buf);
	for (uint64_t i = 0; i < clamped; i++) {
		dst[i] = src[i];
	}
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_close(uint32_t server, uint64_t handle) {
	msg_regs m{};
	m.word[0] = VFS_OP_CLOSE;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_stat(uint32_t server, const char *path, uint64_t *size_out, int *is_dir_out,
                        uint64_t *ino_out) {
	msg_regs m{};
	m.word[0] = VFS_OP_STAT;
	msg_put_str(m, 1, path, VFS_PATH_MAX);
	uint32_t from;
	ipc_call(server, &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0) {
		if (size_out) *size_out = m.word[1];
		if (is_dir_out) *is_dir_out = (int)m.word[2];
		if (ino_out) *ino_out = m.word[3];
	}
	return status;
}

inline int64_t vfs_fstat(uint32_t server, uint64_t handle, uint64_t *size_out, int *is_dir_out,
                         uint64_t *ino_out) {
	msg_regs m{};
	m.word[0] = VFS_OP_FSTAT;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(server, &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0) {
		if (size_out) *size_out = m.word[1];
		if (is_dir_out) *is_dir_out = (int)m.word[2];
		if (ino_out) *ino_out = m.word[3];
	}
	return status;
}

inline int64_t vfs_readdir(uint32_t server, uint64_t dir_ino, uint64_t index, char *name_out,
                           int *is_dir_out) {
	msg_regs m{};
	m.word[0] = VFS_OP_READDIR;
	m.word[1] = dir_ino;
	m.word[2] = index;
	uint32_t from;
	ipc_call(server, &m, &from);
	int64_t status = (int64_t)m.word[0];
	if (status == 0) {
		if (is_dir_out) *is_dir_out = (int)m.word[1];
		const char *src = reinterpret_cast<const char *>(&m.word[2]);
		int i = 0;
		for (; i < VFS_NAME_MAX - 1 && src[i]; i++) {
			name_out[i] = src[i];
		}
		name_out[i] = '\0';
	}
	return status;
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
	uint32_t server_tid; // FD_VFS/FD_VFS_DIR only; opaque to the kernel.
};

// Matches include/robu/spawn.h -- the kernel's spawn handler
// (elf_load_and_spawn_req in src/core/elf.c) special-cases exactly these two
// numeric `kind` values to call pipe_add_holder() for the new child, so the
// pipe's reader/writer bitmask (used for EOF/EPIPE detection) stays correct
// across a spawn. Every other fd `kind` value is opaque to the kernel, just
// copied through for the child's own sysdeps.cpp to interpret -- these two
// are the only ones that need translating to/from this backend's internal
// FdKind enum (sysdeps.cpp) when crossing the spawn wire.
constexpr uint32_t SPAWN_FD_KIND_PIPE_READ = 100;
constexpr uint32_t SPAWN_FD_KIND_PIPE_WRITE = 101;

constexpr int SPAWN_FD_INFO_MAX = 3;
constexpr uint32_t SPAWN_INFO_MAGIC = 0x314e4953u;

// Layout of the fd-info block a spawned child finds via its 5th entry arg
// (spawn_info VA): a robu_spawn_info header immediately followed by
// info.nfds robu_spawn_fd entries.
struct robu_spawn_info {
	uint32_t magic;
	uint32_t nfds;
};

// Shared by robu_spawn() and exec_raw(): both send the identical
// name+argv+envp(+fds) wire format (robu_spawn_req) to the kernel, just
// under different IPC verbs with different kernel-side handling (create a
// new process vs. replace the calling one in place). Returns the built
// request's total length, or -1 on error (buffer too small / too many
// args).
inline int32_t build_spawn_req_buf(uint8_t *buf, const char *name, char *const argv[],
                                    char *const envp[],
                                    bool (*fd_export)(int fd, uint32_t *kind, uint64_t *handle,
                                                       uint32_t *server_tid)) {
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
	if (off + strlen_(name) > SPAWN_REQ_MAX_LEN) return -1;
	uint32_t name_len = (uint32_t)strlen_(name);
	uint32_t name_off = put(name, name_len);
	for (int i = 0; i < argc; i++) {
		uint32_t len = (uint32_t)strlen_(argv[i]);
		if (off + len > SPAWN_REQ_MAX_LEN) return -1;
		argv_table[i].off = put(argv[i], len);
		argv_table[i].len = len;
	}
	for (int i = 0; i < envc; i++) {
		uint32_t len = (uint32_t)strlen_(envp[i]);
		if (off + len > SPAWN_REQ_MAX_LEN) return -1;
		envp_table[i].off = put(envp[i], len);
		envp_table[i].len = len;
	}
	uint32_t argv_off = off;
	if (off + (uint32_t)argc * sizeof(robu_spawn_str) > SPAWN_REQ_MAX_LEN) return -1;
	for (int i = 0; i < argc; i++) {
		reinterpret_cast<robu_spawn_str *>(buf + off)[i] = argv_table[i];
	}
	off += (uint32_t)argc * sizeof(robu_spawn_str);
	uint32_t envp_off = off;
	if (off + (uint32_t)envc * sizeof(robu_spawn_str) > SPAWN_REQ_MAX_LEN) return -1;
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
			uint32_t server_tid = 0;
			if (fd_export(fd, &kind, &handle, &server_tid)) {
				fds[nfds].fd = (uint32_t)fd;
				fds[nfds].kind = kind;
				fds[nfds].handle = handle;
				fds[nfds].server_tid = server_tid;
				nfds++;
			}
		}
	}
	uint32_t fds_off = off;
	if (nfds > 0) {
		if (off + nfds * sizeof(robu_spawn_fd) > SPAWN_REQ_MAX_LEN) return -1;
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
	return (int32_t)off;
}

// fd_kind/fd_handle: caller-supplied lookup for fds 0/1/2, same shape as
// apps/libc's __libc_fd_export -- return false if that fd isn't open.
inline int64_t robu_spawn(const char *name, char *const argv[], char *const envp[],
                           bool (*fd_export)(int fd, uint32_t *kind, uint64_t *handle,
                                              uint32_t *server_tid)) {
	static uint8_t buf[SPAWN_REQ_MAX_LEN];
	int32_t len = build_spawn_req_buf(buf, name, argv, envp, fd_export);
	if (len < 0) {
		return -1;
	}
	msg_regs m{};
	m.word[0] = (uint64_t)buf;
	m.word[1] = (uint64_t)len;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_SPAWN, &m, nullptr);
	if (rc != IPC_ERR_NONE) {
		return -1;
	}
	return (int64_t)m.word[0];
}

// execve(): replaces the calling process's own image in place (same tid,
// same fd table -- it's just process memory that survives the swap, no
// special inheritance logic needed). Never returns on success; returns a
// negative IPC_ERR_* on failure. No fd export -- POSIX exec() inherits
// every fd unless it was marked close-on-exec, and nothing in this backend
// marks any fd close-on-exec, so "inherit everything" already falls out for
// free (the caller's fd table isn't touched by any of this at all).
inline int64_t exec_raw(const char *name, char *const argv[], char *const envp[]) {
	static uint8_t buf[SPAWN_REQ_MAX_LEN];
	int32_t len = build_spawn_req_buf(buf, name, argv, envp, nullptr);
	if (len < 0) {
		return -1;
	}
	msg_regs m{};
	m.word[0] = (uint64_t)buf;
	m.word[1] = (uint64_t)len;
	return ipc_raw(0, 0, IPC_FLAG_EXEC, &m, nullptr);
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

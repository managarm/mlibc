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

constexpr uint64_t DEV_CONSOLE = 0;
constexpr uint64_t DEV_RSTTY1 = 4;
constexpr uint64_t DEV_RSTTY6 = 9;
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
constexpr uint64_t IPC_FLAG_SYS_INFO = 1ull << 23;
constexpr uint64_t IPC_FLAG_THREAD_INFO = 1ull << 22;
constexpr uint64_t SYS_INFO_CAT_SIGACTION = 5;
constexpr uint64_t SYS_INFO_CAT_KILL = 6;
constexpr uint64_t SYS_INFO_CAT_SIGPROCMASK = 7;
constexpr uint64_t SYS_INFO_CAT_SIGRETURN = 8;
constexpr uint64_t SYS_INFO_CAT_SIGPENDING = 9;
constexpr uint64_t SYS_INFO_CAT_CONSOLE_MODE = 10;
constexpr uint64_t SYS_INFO_CAT_SETPGID = 11;
constexpr uint64_t SYS_INFO_CAT_GETPGID = 12;
constexpr uint64_t SYS_INFO_CAT_SETSID = 13;
constexpr uint64_t SYS_INFO_CAT_TCGETPGRP = 14;
constexpr uint64_t SYS_INFO_CAT_TCSETPGRP = 15;
constexpr unsigned long ROBU_TIOCSCTTY = 0x540E;
constexpr unsigned long ROBU_TIOCGPGRP = 0x540F;
constexpr unsigned long ROBU_TIOCSPGRP = 0x5410;
constexpr unsigned long ROBU_TIOCGWINSZ = 0x5413;

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

inline int64_t fork_raw(uint64_t *out_child_or_zero) {
	msg_regs m{};
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_FORK, &m, nullptr);
	*out_child_or_zero = m.word[0];
	return rc;
}

inline uint64_t self_tid() {
	msg_regs m{};
	ipc_raw(0, 0, IPC_FLAG_SELF_TID, &m, nullptr);
	return m.word[0];
}

inline int64_t sig_action_raw(int signum, uint64_t new_handler, uint64_t new_flags,
                               uint64_t new_mask, int do_set,
                               uint64_t *old_handler, uint64_t *old_flags, uint64_t *old_mask) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_SIGACTION;
	m.word[1] = (uint64_t)signum;
	m.word[2] = new_handler;
	m.word[3] = new_flags;
	m.word[4] = (uint64_t)do_set;
	m.word[5] = new_mask;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	if (old_handler) *old_handler = m.word[0];
	if (old_flags) *old_flags = m.word[1];
	if (old_mask) *old_mask = m.word[2];
	return rc;
}

inline int64_t sig_kill_raw(uint64_t target_tid, int signum) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_KILL;
	m.word[1] = target_tid;
	m.word[2] = (uint64_t)signum;
	return ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
}

inline int64_t sig_procmask_raw(uint64_t how, uint64_t new_mask, int do_set, uint64_t *old_mask) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_SIGPROCMASK;
	m.word[1] = how;
	m.word[2] = new_mask;
	m.word[3] = (uint64_t)do_set;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	if (old_mask) *old_mask = m.word[0];
	return rc;
}

inline uint64_t sig_pending_raw() {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_SIGPENDING;
	ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	return m.word[0];
}

inline void sleep_raw(uint64_t ticks) {
	ipc_raw(0, ticks, IPC_FLAG_NONE, nullptr, nullptr);
}

inline int64_t console_mode_query_raw(int vt) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_CONSOLE_MODE;
	m.word[1] = 2;
	m.word[2] = (uint64_t)vt;
	ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	return (int64_t)m.word[0];
}

inline void console_mode_set_raw(int vt, int enable) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_CONSOLE_MODE;
	m.word[1] = (uint64_t)enable;
	m.word[2] = (uint64_t)vt;
	ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
}

inline int64_t setpgid_raw(uint64_t target_tid, uint64_t new_pgid) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_SETPGID;
	m.word[1] = target_tid;
	m.word[2] = new_pgid;
	return ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
}

inline int64_t getpgid_raw(uint64_t target_tid, uint64_t *out_pgid) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_GETPGID;
	m.word[1] = target_tid;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	if (out_pgid) *out_pgid = m.word[0];
	return rc;
}

inline int64_t setsid_raw(uint64_t *out_sid) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_SETSID;
	int64_t rc = ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	if (out_sid) *out_sid = m.word[0];
	return rc;
}

inline uint64_t tcgetpgrp_raw() {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_TCGETPGRP;
	ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
	return m.word[0];
}

inline void tcsetpgrp_raw(uint64_t pgid) {
	msg_regs m{};
	m.word[0] = SYS_INFO_CAT_TCSETPGRP;
	m.word[1] = pgid;
	ipc_raw(0, 0, IPC_FLAG_SYS_INFO, &m, nullptr);
}

constexpr uint64_t KINFO_VA = 0x0000000080000000ULL;

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

constexpr uint64_t VFS_OP_OPEN    = 1;
constexpr uint64_t VFS_OP_READ    = 2;
constexpr uint64_t VFS_OP_WRITE   = 3;
constexpr uint64_t VFS_OP_CLOSE   = 4;
constexpr uint64_t VFS_OP_STAT    = 5;
constexpr uint64_t VFS_OP_FSTAT   = 6;
constexpr uint64_t VFS_OP_READDIR = 7;
constexpr uint64_t VFS_OP_RENAME  = 8;
constexpr uint64_t VFS_OP_UNLINK  = 9;
constexpr uint64_t VFS_OP_QUIESCE = 11;
constexpr uint64_t VFS_OP_PEEK    = 12;
constexpr uint64_t VFS_OP_MKDIR   = 13;
constexpr uint64_t VFS_OP_RMDIR   = 14;
constexpr uint64_t VFS_OP_LINK    = 15;
constexpr uint64_t VFS_OP_MKNOD   = 16;
constexpr int64_t VFS_ERR_NOT_FOUND     = -1;
constexpr int64_t VFS_ERR_BAD_HANDLE    = -2;
constexpr int64_t VFS_ERR_NOT_SUPPORTED = -3;
constexpr int64_t VFS_ERR_NO_SPACE      = -4;
constexpr int64_t VFS_ERR_IS_DIR        = -5;
constexpr int64_t VFS_ERR_EXISTS        = -6;
constexpr int64_t VFS_ERR_NOT_DIR       = -7;
constexpr int64_t VFS_ERR_NOT_EMPTY     = -8;
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

inline int64_t vfs_peek(uint32_t server, uint64_t handle) {
	msg_regs m{};
	m.word[0] = VFS_OP_PEEK;
	m.word[1] = handle;
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
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

inline int64_t vfs_unlink(uint32_t server, const char *name) {
	msg_regs m{};
	m.word[0] = VFS_OP_UNLINK;
	msg_put_str(m, 1, name, VFS_PATH_MAX);
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_mkdir(uint32_t server, const char *name) {
	msg_regs m{};
	m.word[0] = VFS_OP_MKDIR;
	msg_put_str(m, 1, name, VFS_PATH_MAX);
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_rmdir(uint32_t server, const char *name) {
	msg_regs m{};
	m.word[0] = VFS_OP_RMDIR;
	msg_put_str(m, 1, name, VFS_PATH_MAX);
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_link(uint32_t server, const char *oldname, const char *newname) {
	msg_regs m{};
	m.word[0] = VFS_OP_LINK;
	char *bytes = reinterpret_cast<char *>(&m);
	int i = 0;
	for (; i < VFS_NAME_MAX - 1 && oldname[i]; i++) bytes[8 + i] = oldname[i];
	bytes[8 + i] = '\0';
	i = 0;
	for (; i < VFS_NAME_MAX - 1 && newname[i]; i++) bytes[8 + VFS_NAME_MAX + i] = newname[i];
	bytes[8 + VFS_NAME_MAX + i] = '\0';
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_mknod(uint32_t server, const char *name, uint64_t mode, uint64_t dev) {
	msg_regs m{};
	m.word[0] = VFS_OP_MKNOD;
	m.word[1] = mode;
	m.word[2] = dev;
	msg_put_str(m, 3, name, VFS_NAME_MAX);
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline int64_t vfs_quiesce(uint32_t server) {
	msg_regs m{};
	m.word[0] = VFS_OP_QUIESCE;
	uint32_t from;
	ipc_call(server, &m, &from);
	return (int64_t)m.word[0];
}

inline void vfs_quiesce_disk() {
	uint32_t tid = resolve_mount("/mnt/disk0/", nullptr);
	if (tid != 0) {
		vfs_quiesce(tid);
	}
}

constexpr uint64_t RAMFS_OP_STAT    = 5;
constexpr uint64_t RAMFS_OP_READDIR = 7;
constexpr int RAMFS_NAME_MAX  = 20;
constexpr int RAMFS_PATH_MAX  = 32;
constexpr uint64_t RAMFS_ROOT_INO = 1;

inline uint32_t ramfs_tid() {
	return kinfo()->ramfs_tid;
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

inline uint64_t strlen_(const char *s) {
	uint64_t n = 0;
	while (s[n]) n++;
	return n;
}

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
	uint32_t server_tid;
};

constexpr uint32_t SPAWN_FD_KIND_PIPE_READ = 100;
constexpr uint32_t SPAWN_FD_KIND_PIPE_WRITE = 101;

constexpr int SPAWN_FD_INFO_MAX = 3;
constexpr uint32_t SPAWN_INFO_MAGIC = 0x314e4953u;

struct robu_spawn_info {
	uint32_t magic;
	uint32_t nfds;
};

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

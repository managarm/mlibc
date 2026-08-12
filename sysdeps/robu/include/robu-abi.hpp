#pragma once

#include <stdint.h>

namespace robu {

struct msg_regs {
	uint64_t word[6];
};

constexpr uint64_t IPC_TID_ANY = 0;
constexpr uint64_t IPC_FLAG_NONE = 0;
constexpr uint64_t IPC_FLAG_RECV = 1ull << 1;
constexpr uint64_t IPC_FLAG_CONSOLE_WRITE = 1ull << 7;
constexpr uint64_t IPC_FLAG_CONSOLE_READ = 1ull << 17;
constexpr uint64_t IPC_FLAG_EXIT = 1ull << 14;
constexpr uint64_t IPC_FLAG_SET_FSBASE = 1ull << 25;

constexpr int64_t IPC_ERR_NONE = 0;

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

}

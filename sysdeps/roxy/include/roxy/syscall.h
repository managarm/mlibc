#ifndef _ROXY_SYSCALL_H
#define _ROXY_SYSCALL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long roxy_syscall_word_t;

#define ROXY_SYS_EXIT 0
#define ROXY_SYS_READ 1
#define ROXY_SYS_WRITE 2
#define ROXY_SYS_FUTEX_WAIT 3
#define ROXY_SYS_FUTEX_WAKE 4
#define ROXY_SYS_ANON_ALLOCATE 5
#define ROXY_SYS_ANON_FREE 6
#define ROXY_SYS_TCB_SET 7
#define ROXY_SYS_CLOCK_GET 8
#define ROXY_SYS_VM_MAP 9
#define ROXY_SYS_VM_UNMAP 10
#define ROXY_SYS_CLOSE 11
#define ROXY_SYS_SEEK 12
#define ROXY_SYS_ISATTY 13
#define ROXY_SYS_OPEN 14
#define ROXY_SYS_VM_PROTECT 15
#define ROXY_SYS_STAT 16
#define ROXY_SYS_FORK 17
#define ROXY_SYS_EXECVE 18
#define ROXY_SYS_GETPID 19
#define ROXY_SYS_GETPPID 20
#define ROXY_SYS_GETEUID 21
#define ROXY_SYS_GETUID 22
#define ROXY_SYS_GETGID 23

typedef struct {
	int64_t seconds;
	int64_t nanoseconds;
} roxy_clock_result;

typedef struct {
	uint64_t file_id;
	uint64_t size;
	uint64_t blocks;
	uint64_t hard_links;
	uint32_t mode;
	uint32_t block_size;
} roxy_stat_result;

#ifdef __cplusplus
static_assert(sizeof(roxy_clock_result) == 16);
static_assert(alignof(roxy_clock_result) == 8);
static_assert(offsetof(roxy_clock_result, seconds) == 0);
static_assert(offsetof(roxy_clock_result, nanoseconds) == 8);
static_assert(sizeof(roxy_stat_result) == 40);
static_assert(alignof(roxy_stat_result) == 8);
static_assert(offsetof(roxy_stat_result, file_id) == 0);
static_assert(offsetof(roxy_stat_result, size) == 8);
static_assert(offsetof(roxy_stat_result, blocks) == 16);
static_assert(offsetof(roxy_stat_result, hard_links) == 24);
static_assert(offsetof(roxy_stat_result, mode) == 32);
static_assert(offsetof(roxy_stat_result, block_size) == 36);
#endif

roxy_syscall_word_t roxy_syscall0(long number);

roxy_syscall_word_t roxy_syscall1(long number, roxy_syscall_word_t first);

roxy_syscall_word_t roxy_syscall2(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second
);

roxy_syscall_word_t roxy_syscall3(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third
);

roxy_syscall_word_t roxy_syscall4(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third,
	roxy_syscall_word_t fourth
);

roxy_syscall_word_t roxy_syscall5(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third,
	roxy_syscall_word_t fourth,
	roxy_syscall_word_t fifth
);

roxy_syscall_word_t roxy_syscall6(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third,
	roxy_syscall_word_t fourth,
	roxy_syscall_word_t fifth,
	roxy_syscall_word_t sixth
);

#ifdef __cplusplus
}
#endif

#endif

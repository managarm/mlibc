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
#define ROXY_SYS_GETEGID 24
#define ROXY_SYS_WAITPID 25
#define ROXY_SYS_SIGPROCMASK 26
#define ROXY_SYS_SIGACTION 27
#define ROXY_SYS_OPEN_DIR 28
#define ROXY_SYS_READ_ENTRIES 29
#define ROXY_SYS_CHDIR 30
#define ROXY_SYS_IOCTL 31
#define ROXY_SYS_GETCWD 32
#define ROXY_SYS_POLL 33
#define ROXY_SYS_SLEEP 34
#define ROXY_SYS_SEND_SIGNAL 35
#define ROXY_SYS_PPOLL 36
#define ROXY_SYS_PSELECT 37
#define ROXY_SYS_UNAME 38
#define ROXY_SYS_MKDIRAT 39
#define ROXY_SYS_UNLINKAT 40
#define ROXY_SYS_READLINKAT 41
#define ROXY_SYS_LINKAT 42
#define ROXY_SYS_SYMLINKAT 43
#define ROXY_SYS_RENAMEAT 44
#define ROXY_SYS_SYNC 45
#define ROXY_SYS_FSYNC 46
#define ROXY_SYS_FTRUNCATE 47
#define ROXY_SYS_SOCKETPAIR 48
#define ROXY_SYS_SOCKET 49
#define ROXY_SYS_BIND 50
#define ROXY_SYS_LISTEN 51
#define ROXY_SYS_ACCEPT 52
#define ROXY_SYS_CONNECT 53
#define ROXY_SYS_SOCKNAME 61
#define ROXY_SYS_PEERNAME 62
#define ROXY_SYS_SHUTDOWN 63
#define ROXY_SYS_GETSOCKOPT 64
#define ROXY_SYS_ACCESS 65
#define ROXY_SYS_RECVMSG 66
#define ROXY_SYS_SENDMSG 67
#define ROXY_SYS_PIPE 55
#define ROXY_SYS_DUP2 56
#define ROXY_SYS_FCNTL 57
#define ROXY_SYS_UMASK 58
#define ROXY_SYS_CHMOD 59
#define ROXY_SYS_FCHMOD 60
#define ROXY_SYS_SET_PGID 68
#define ROXY_SYS_GET_PGID 69
#define ROXY_SYS_SET_SID 70

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

typedef struct {
	uint64_t inode;
	int64_t offset;
	uint16_t record_size;
	uint8_t type;
	char name[256];
	uint8_t padding[5];
} roxy_dirent;

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
static_assert(sizeof(roxy_dirent) == 280);
static_assert(alignof(roxy_dirent) == 8);
static_assert(offsetof(roxy_dirent, inode) == 0);
static_assert(offsetof(roxy_dirent, offset) == 8);
static_assert(offsetof(roxy_dirent, record_size) == 16);
static_assert(offsetof(roxy_dirent, type) == 18);
static_assert(offsetof(roxy_dirent, name) == 19);
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

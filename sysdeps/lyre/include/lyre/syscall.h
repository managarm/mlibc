#ifndef _LYRE__SYSCALL_H
#define _LYRE__SYSCALL_H

#include <stdint.h>

#define SYS_debug 0
#define SYS_mmap 1
#define SYS_openat 2
#define SYS_close 3
#define SYS_read 4
#define SYS_write 5
#define SYS_seek 6
#define SYS_set_fs_base 7
#define SYS_set_gs_base 8
#define SYS_stat 9
#define SYS_fcntl 10
#define SYS_dup3 11
#define SYS_ioctl 12
#define SYS_fork 13
#define SYS_exec 14
#define SYS_getpid 15
#define SYS_waitpid 16
#define SYS_exit 17
#define SYS_getcwd 18
#define SYS_chdir 19
#define SYS_unmmap 20
#define SYS_pipe 21
#define SYS_readlinkat 22
#define SYS_linkat 23
#define SYS_unlinkat 24
#define SYS_readdir 25
#define SYS_uname 26
#define SYS_futex_wait 27
#define SYS_futex_wake 28
#define SYS_mkdirat 29
#define SYS_fchmodat 30
#define SYS_sleep 31
#define SYS_ppoll 32
#define SYS_umask 33
#define SYS_getmemstat 34
#define SYS_getclock 35
#define SYS_socket 36
#define SYS_bind 37
#define SYS_connect 38
#define SYS_listen 39
#define SYS_accept 40
#define SYS_getpeername 41
#define SYS_recvmsg 42

struct __syscall_ret {
	uint64_t ret;
	uint64_t errno;
};

#define __SYSCALL_EXPAND(...) \
    struct __syscall_ret ret; \
	asm volatile ( \
		"mov %%rsp, %%r10\n\t" \
		"lea 1f(%%rip), %%r11\n\t" \
		"sysenter\n\t" \
		"1:" \
		: "=a"(ret.ret), "=b"(ret.errno) __VA_ARGS__ \
		"r10", "r11", "memory" \
	); \
	return ret

static inline struct __syscall_ret __syscall5(int number, uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e) {
    register uint64_t r8 asm("%r8") = d;
    register uint64_t r9 asm("%r9") = e;
    __SYSCALL_EXPAND(, "+d"(b), "+c"(c) : "D"(number), "S"(a), "r"(r8), "r"(r9) :);
}

static inline struct __syscall_ret __syscall4(int number, uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    register uint64_t r8 asm("%r8") = d;
    __SYSCALL_EXPAND(, "+d"(b), "+c"(c) : "D"(number), "S"(a), "r"(r8) :);
}

static inline struct __syscall_ret __syscall3(int number, uint64_t a, uint64_t b, uint64_t c) {
    __SYSCALL_EXPAND(, "+d"(b), "+c"(c) : "D"(number), "S"(a) :);
}

static inline struct __syscall_ret __syscall2(int number, uint64_t a, uint64_t b) {
    __SYSCALL_EXPAND(, "+d"(b) : "D"(number), "S"(a) : "rcx", );
}

static inline struct __syscall_ret __syscall1(int number, uint64_t a) {
    __SYSCALL_EXPAND( : "D"(number), "S"(a) : "rcx", "rdx", );
}

static inline struct __syscall_ret __syscall0(int number) {
    __SYSCALL_EXPAND( : "D"(number) : "rcx", "rdx", );
}

#define __SYSCALL_NARGS_SEQ(_0,_1,_2,_3,_4,_5,_6,_7,N,...) N
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_SEQ(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)

#define __SYSCALL_CONCAT1(X, Y) X##Y
#define __SYSCALL_CONCAT(X, Y) __SYSCALL_CONCAT1(X, Y)

#define __syscall(...) ({ \
    struct __syscall_ret (*__SYSCALL_f)(int, ...); \
    __SYSCALL_f = (struct __syscall_ret (*)(int, ...))__SYSCALL_CONCAT(__syscall, __SYSCALL_NARGS(__VA_ARGS__)); \
    __SYSCALL_f(__VA_ARGS__); \
})

#endif

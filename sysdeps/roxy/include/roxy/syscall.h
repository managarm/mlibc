#ifndef _ROXY_SYSCALL_H
#define _ROXY_SYSCALL_H

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

roxy_syscall_word_t roxy_syscall1(long number, roxy_syscall_word_t first);

roxy_syscall_word_t roxy_syscall3(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third
);

#ifdef __cplusplus
}
#endif

#endif

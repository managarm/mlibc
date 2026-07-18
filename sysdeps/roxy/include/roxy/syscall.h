#ifndef _ROXY_SYSCALL_H
#define _ROXY_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef long roxy_syscall_word_t;

#define ROXY_SYS_READ 1
#define ROXY_SYS_WRITE 2

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

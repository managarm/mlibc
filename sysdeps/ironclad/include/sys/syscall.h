#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stddef.h>

#define SYSCALL0(NUM) ({ \
	asm volatile ("int $0x80" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM) \
				  : "memory"); \
})

#define SYSCALL1(NUM, ARG0) ({ \
	asm volatile ("int $0x80" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0) \
				  : "memory"); \
})

#define SYSCALL2(NUM, ARG0, ARG1) ({ \
	asm volatile ("int $0x80" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1) \
				  : "memory"); \
})

#define SYSCALL3(NUM, ARG0, ARG1, ARG2) ({ \
	asm volatile ("int $0x80" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2) \
				  : "memory"); \
})

#define SYSCALL4(NUM, ARG0, ARG1, ARG2, ARG3) ({ \
	asm volatile ("int $0x80" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), "c"(ARG3) \
				  : "memory"); \
})

#define SYSCALL6(NUM, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5) ({ \
	register uint64_t arg_r8 asm("r8") = (uint64_t)ARG4; \
	register uint64_t arg_r9 asm("r9") = (uint64_t)ARG5; \
	asm volatile ("int $0x80" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), \
					"c"(ARG3), "r"(arg_r8), "r"(arg_r9) \
				  : "memory"); \
})

#define SYSCALL_EXIT               0
#define SYSCALL_SET_TCB            1
#define SYSCALL_OPEN               2
#define SYSCALL_CLOSE              3
#define SYSCALL_READ               4
#define SYSCALL_WRITE              5
#define SYSCALL_SEEK               6
#define SYSCALL_MMAP               7
#define SYSCALL_MUNMAP             8
#define SYSCALL_GETPID             9
#define SYSCALL_GETPPID           10
#define SYSCALL_EXEC              11
#define SYSCALL_FORK              12
#define SYSCALL_WAIT              13
#define SYSCALL_UNAME             14
#define SYSCALL_SETHOSTNAME       15
#define SYSCALL_FSTAT             16
#define SYSCALL_LSTAT             17
#define SYSCALL_GETCWD            18
#define SYSCALL_CHDIR             19
#define SYSCALL_IOCTL             20
#define SYSCALL_SCHED_YIELD       21
#define SYSCALL_GETPRIORITY       22
#define SYSCALL_SETPRIORITY       23
#define SYSCALL_DUP               24
#define SYSCALL_DUP2              25
#define SYSCALL_DUP3              26
#define SYSCALL_ACCESS            27

#endif // _SYSCALL_H

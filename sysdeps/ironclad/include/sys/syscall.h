#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <stddef.h>

#define SYSCALL0(NUM) ({ \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL1(NUM, ARG0) ({ \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL2(NUM, ARG0, ARG1) ({ \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL3(NUM, ARG0, ARG1, ARG2) ({ \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL4(NUM, ARG0, ARG1, ARG2, ARG3) ({ \
	register __typeof(ARG3) arg_r12 asm("r12") = ARG3; \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), \
					"r"(arg_r12) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL5(NUM, ARG0, ARG1, ARG2, ARG3, ARG4) ({ \
	register __typeof(ARG3) arg_r12 asm("r12") = ARG3; \
	register __typeof(ARG4) arg_r8 asm("r8") = ARG4; \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), \
					"r"(arg_r12), "r"(arg_r8) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL6(NUM, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5) ({ \
	register __typeof(ARG3) arg_r12 asm("r12") = ARG3; \
	register __typeof(ARG4) arg_r8 asm("r8") = ARG4; \
	register __typeof(ARG5) arg_r9 asm("r9") = ARG5; \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), \
					"r"(arg_r12), "r"(arg_r8), "r"(arg_r9) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL6(NUM, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5) ({ \
	register __typeof(ARG3) arg_r12 asm("r12") = ARG3; \
	register __typeof(ARG4) arg_r8  asm("r8")  = ARG4; \
	register __typeof(ARG5) arg_r9  asm("r9")  = ARG5; \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), \
					"r"(arg_r12), "r"(arg_r8), "r"(arg_r9) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL7(NUM, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) ({ \
	register __typeof(ARG3) arg_r12 asm("r12") = ARG3; \
	register __typeof(ARG4) arg_r8  asm("r8")  = ARG4; \
	register __typeof(ARG5) arg_r9  asm("r9")  = ARG5; \
	register __typeof(ARG6) arg_r10 asm("r10") = ARG6; \
	asm volatile ("syscall" \
				  : "=a"(ret), "=d"(errno) \
				  : "a"(NUM), "D"(ARG0), "S"(ARG1), "d"(ARG2), \
					"r"(arg_r12), "r"(arg_r8), "r"(arg_r9), "r"(arg_r10) \
				  : "rcx", "r11", "memory"); \
})

#define SYSCALL_EXIT                  0
#define SYSCALL_ARCH_PRCTL            1
#define SYSCALL_OPEN                  2
#define SYSCALL_CLOSE                 3
#define SYSCALL_READ                  4
#define SYSCALL_WRITE                 5
#define SYSCALL_SEEK                  6
#define SYSCALL_MMAP                  7
#define SYSCALL_MUNMAP                8
#define SYSCALL_GETPID                9
#define SYSCALL_GETPPID              10
#define SYSCALL_EXEC                 11
#define SYSCALL_CLONE                12
#define SYSCALL_WAIT                 13
#define SYSCALL_SOCKET               14
#define SYSCALL_SETHOSTNAME          15
#define SYSCALL_UNLINK               16
#define SYSCALL_STAT                 17
#define SYSCALL_GETCWD               18
#define SYSCALL_CHDIR                19
#define SYSCALL_IOCTL                20
#define SYSCALL_SCHED_YIELD          21
#define SYSCALL_DELETE_TCLUSTER      22
#define SYSCALL_PIPE                 23
#define SYSCALL_GETUID               24
#define SYSCALL_RENAME               25
#define SYSCALL_SYSCONF              26
#define SYSCALL_SPAWN                27
#define SYSCALL_GETTID               28
#define SYSCALL_MANAGE_TCLUSTER      29
#define SYSCALL_FCNTL                30
#define SYSCALL_EXIT_THREAD          31
#define SYSCALL_GETRANDOM            32
#define SYSCALL_MPROTECT             33
#define SYSCALL_SYNC                 34
#define SYSCALL_SET_MAC_CAPABILITIES 35
#define SYSCALL_GET_MAC_CAPABILITIES 36
#define SYSCALL_ADD_MAC_PERMISSIONS  37
#define SYSCALL_SET_MAC_ENFORCEMENT  38
#define SYSCALL_MOUNT                39
#define SYSCALL_UMOUNT               40
#define SYSCALL_READLINK             41
#define SYSCALL_GETDENTS             42
#define SYSCALL_MAKENODE             43
#define SYSCALL_TRUNCATE             44
#define SYSCALL_BIND                 45
#define SYSCALL_SYMLINK              46
#define SYSCALL_CONNECT              47
#define SYSCALL_OPENPTY              48
#define SYSCALL_FSYNC                49
#define SYSCALL_LINK                 50
#define SYSCALL_PTRACE               51
#define SYSCALL_LISTEN               52
#define SYSCALL_ACCEPT               53
#define SYSCALL_GETRLIMIT            54
#define SYSCALL_SETRLIMIT            55
#define SYSCALL_ACCESS               56
#define SYSCALL_POLL                 57
#define SYSCALL_GETEUID              58
#define SYSCALL_SETUIDS              59
#define SYSCALL_FCHMOD               60
#define SYSCALL_UMASK                61
#define SYSCALL_REBOOT               62
#define SYSCALL_FCHOWN               63
#define SYSCALL_PREAD                64
#define SYSCALL_PWRITE               65
#define SYSCALL_GETSOCKNAME          66
#define SYSCALL_GETPEERNAME          67
#define SYSCALL_SHUTDOWN             68
#define SYSCALL_FUTEX                69
#define SYSCALL_CLOCK                70
#define SYSCALL_CLOCK_NANOSLEEP      71
#define SYSCALL_GETRUSAGE            72
#define SYSCALL_RECVFROM             73
#define SYSCALL_SENDTO               74
#define SYSCALL_CONFIG_NETINTER      75
#define SYSCALL_UTIMES               76
#define SYSCALL_CREATE_TCLUSTER      77
#define SYSCALL_SWITCH_TCLUSTER      78
#define SYSCALL_ACTUALLY_KILL        79
#define SYSCALL_SIGNALPOST           80
#define SYSCALL_SEND_SIGNAL          81

#endif // _SYS_SYSCALL_H

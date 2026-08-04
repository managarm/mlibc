#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* 0-63: POSIX Core */
#define SYS_EXIT        0
#define SYS_FORK        1
#define SYS_EXECVE     2
#define SYS_WAITPID    3
#define SYS_GETPID     4
#define SYS_GETPPID    5

#define SYS_OPEN       6
#define SYS_CLOSE      7
#define SYS_READ       8
#define SYS_WRITE      9
#define SYS_PREAD     10
#define SYS_PWRITE    11
#define SYS_LSEEK     12
#define SYS_DUP       13
#define SYS_IOCTL     14

#define SYS_STAT      15
#define SYS_FSTAT     16
#define SYS_READLINK  17
#define SYS_CHMOD     18
#define SYS_CHDIR     19
#define SYS_FCHDIR    20
#define SYS_GETCWD    21
#define SYS_RENAME    22
#define SYS_LINK      23
#define SYS_UNLINK    24
#define SYS_MKDIR     25
#define SYS_RMDIR     26

#define SYS_PIPE      27

#define SYS_CLOCK_GETTIME 28
#define SYS_NANOSLEEP     29
#define SYS_ALARM         30

#define SYS_GETUID    31
#define SYS_GETEUID   32
#define SYS_SETUID    33
#define SYS_GETGID    34
#define SYS_GETEGID   35

#define SYS_UNAME     36

/* 64-127: Memory / Threads / Signals */

#define SYS_MMAP          64
#define SYS_MUNMAP        65

#define SYS_SET_FS_BASE   66

#define SYS_SPAWN_THREAD  67
#define SYS_EXIT_THREAD   68
#define SYS_GETTID        69
#define SYS_YIELD         70

#define SYS_FUTEX_WAIT    71
#define SYS_FUTEX_WAKE    72

#define SYS_SIGNAL_ACTION 73
#define SYS_SIGPROCMASK   74
#define SYS_KILL          75
#define SYS_SIGNAL_RETURN 76

#define SYS_GET_RESOURCE_LIMIT 77


/* 128-191: Networking */
#define SYS_SOCKET        128
#define SYS_SOCKETPAIR    129
#define SYS_BIND          130
#define SYS_LISTEN        131
#define SYS_ACCEPT        132
#define SYS_CONNECT       133

#define SYS_SEND          134
#define SYS_SENDTO        135
#define SYS_SENDMSG       136

#define SYS_RECEIVE       137
#define SYS_RECEIVEFROM   138
#define SYS_RECVMSG       139

#define SYS_PEERNAME      140
#define SYS_SOCKNAME      141
#define SYS_SET_SOCKET_OPTIONS 142
#define SYS_GET_SOCKET_OPTIONS 143

/* 192-255: Polling / Multiplexing */

#define SYS_POLL          192
#define SYS_SELECT        193

#define SYS_EPOLL_CREATE  194
#define SYS_EPOLL_CTL     195
#define SYS_EPOLL_WAIT    196

/* 256-383: Kernel Objects / IPC */
#define SYS_CREATE_SHARED_MEMORY   256
#define SYS_MAP_SHARED_MEMORY      257
#define SYS_UNMAP_SHARED_MEMORY    258
#define SYS_DESTROY_SHARED_MEMORY  259

#define SYS_CREATE_SERVICE         260
#define SYS_CREATE_INTERFACE       261
#define SYS_INTERFACE_ACCEPT       262
#define SYS_INTERFACE_CONNECT      263

#define SYS_ENDPOINT_QUEUE         264
#define SYS_ENDPOINT_DEQUEUE       265
#define SYS_ENDPOINT_CALL          266
#define SYS_ENDPOINT_INFO          267

#define SYS_KERNELOBJECT_WAIT_ONE  268
#define SYS_KERNELOBJECT_WAIT      269
#define SYS_KERNELOBJECT_DESTROY   270

#define SYS_SEND_MESSAGE           271
#define SYS_RECEIVE_MESSAGE        272


/* 384-511: Device / System */
#define SYS_MOUNT            384
#define SYS_UNMOUNT				385
#define SYS_DEVICE_MANAGEMENT 386

#define SYS_MAP_FB           387
#define SYS_GET_VIDEO_MODE   388

#define SYS_UPTIME           389
#define SYS_TIME             390
#define SYS_INFO             391

#define SYS_GET_PROCESS_INFO       392
#define SYS_GET_NEXT_PROCESS_INFO  393

#define SYS_GETENTROPY       394

#define SYS_LOAD_KERNEL_MODULE     395
#define SYS_UNLOAD_KERNEL_MODULE   396

/* 512+: Misc. */
/* Nothing is in this range yet. */


#ifdef __cplusplus
extern "C" {
#endif

	__attribute__((__always_inline__))
		static inline long syscalln0(uint64_t call) {
		volatile long ret;
		asm volatile("int $0x42" : "=a"(ret) : "a"(call));
		return ret;
	}

	__attribute__((__always_inline__))
		static inline long syscalln1(uint64_t call, uint64_t arg0) {
		volatile long ret;
		asm volatile("int $0x42" : "=a"(ret) : "a"(call), "D"(arg0) : "memory");
		return ret;
	}

	__attribute__((__always_inline__))
		static inline long syscalln2(uint64_t call, uint64_t arg0, uint64_t arg1) {
		volatile long ret;
		asm volatile("int $0x42" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1) : "memory");
		return ret;
	}

	__attribute__((__always_inline__))
		static inline long syscalln3(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
		volatile long ret;
		asm volatile("int $0x42" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2) : "memory");
		return ret;
	}

	__attribute__((__always_inline__))
		static inline long syscalln4(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
		volatile long ret;
		register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
		asm volatile("int $0x42" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r) : "memory");
		return ret;
	}

	__attribute__((__always_inline__))
		static inline long syscalln5(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
		volatile long ret;
		register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
		register uint64_t arg4r asm("r9") = arg4; // put arg4 in r9
		asm volatile("int $0x42" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r) : "memory");
		return ret;
	}

	__attribute__((__always_inline__))
		static inline long syscalln6(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
		volatile long ret;
		register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
		register uint64_t arg4r asm("r9") = arg4; // put arg4 in r9
		register uint64_t arg5r asm("r8") = arg5; // put arg5 in r8
		asm volatile("int $0x42" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r), "r"(arg5r) : "memory");
		return ret;
	}

#ifdef __cplusplus
}
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call) { return syscalln0(call); }
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0) { return syscalln1(call, arg0); }
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1) { return syscalln2(call, arg0, arg1); }
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) { return syscalln3(call, arg0, arg1, arg2); }
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) { return syscalln4(call, arg0, arg1, arg2, arg3); }
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) { return syscalln5(call, arg0, arg1, arg2, arg3, arg4); }
__attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) { return syscalln6(call, arg0, arg1, arg2, arg3, arg4, arg5); }

template<typename... T>
__attribute__((__always_inline__)) static inline long syscall(uint64_t call, T... args) {
	return _syscall(call, (uint64_t) (args)...);
}
#else
#define GET_SYSCALL(a0, a1, a2, a3, a4, a5, a6, name, ...) name
#define syscall(...) GET_SYSCALL(__VA_ARGS__, syscalln6, syscalln5, syscalln4, syscalln3, syscalln2, syscalln1, syscalln0)(__VA_ARGS__)
#endif

#endif // SYSCALL_H
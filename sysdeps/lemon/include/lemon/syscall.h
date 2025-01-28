#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_EXEC 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_SLEEP 7
#define SYS_CREATE 8
#define SYS_LINK 9
#define SYS_UNLINK 10
#define SYS_EXECVE 11
#define SYS_CHDIR 12
#define SYS_TIME 13
#define SYS_MAP_FB 14
#define SYS_GETTID 15
#define SYS_CHMOD 16
#define SYS_FSTAT 17
#define SYS_STAT 18
#define SYS_LSEEK 19
#define SYS_GETPID 20
#define SYS_MOUNT 21
#define SYS_MKDIR 22
#define SYS_RMDIR 23
#define SYS_RENAME 24
#define SYS_YIELD 25
#define SYS_READDIR_NEXT 26
#define SYS_SEND_MESSAGE 28
#define SYS_RECEIVE_MESSAGE 29
#define SYS_UPTIME 30
#define SYS_GET_VIDEO_MODE 31
#define SYS_UNAME 32
#define SYS_READDIR 33
#define SYS_SET_FS_BASE 34
#define SYS_MMAP 35
#define SYS_GET_CWD 37
#define SYS_WAIT_PID 38
#define SYS_NANO_SLEEP 39
#define SYS_PREAD 40
#define SYS_PWRITE 41
#define SYS_IOCTL 42
#define SYS_INFO 43
#define SYS_MUNMAP 44
#define SYS_CREATE_SHARED_MEMORY 45
#define SYS_MAP_SHARED_MEMORY 46
#define SYS_UNMAP_SHARED_MEMORY 47
#define SYS_DESTROY_SHARED_MEMORY 48
#define SYS_SOCKET 49
#define SYS_BIND 50
#define SYS_LISTEN 51
#define SYS_ACCEPT 52
#define SYS_CONNECT 53
#define SYS_SEND 54
#define SYS_SENDTO 55
#define SYS_RECEIVE 56
#define SYS_RECEIVEFROM 57
#define SYS_GETUID 58
#define SYS_SETUID 59
#define SYS_POLL 60
#define SYS_SENDMSG 61
#define SYS_RECVMSG 62
#define SYS_GETEUID 63
#define SYS_SETEUID 64
#define SYS_GET_PROCESS_INFO 65
#define SYS_GET_NEXT_PROCESS_INFO 66
#define SYS_READLINK 67
#define SYS_SPAWN_THREAD 68
#define SYS_EXIT_THREAD 69
#define SYS_FUTEX_WAKE 70
#define SYS_FUTEX_WAIT 71
#define SYS_DUP 72
#define SYS_GET_FILE_STATUS_FLAGS 73
#define SYS_SET_FILE_STATUS_FLAGS 74
#define SYS_SELECT 75
#define SYS_CREATE_SERVICE 76
#define SYS_CREATE_INTERFACE 77
#define SYS_INTERFACE_ACCEPT 78
#define SYS_INTERFACE_CONNECT 79
#define SYS_ENDPOINT_QUEUE 80
#define SYS_ENDPOINT_DEQUEUE 81
#define SYS_ENDPOINT_CALL 82
#define SYS_ENDPOINT_INFO 83
#define SYS_KERNELOBJECT_WAIT_ONE 84
#define SYS_KERNELOBJECT_WAIT 85
#define SYS_KERNELOBJECT_DESTROY 86
#define SYS_SET_SOCKET_OPTIONS 87
#define SYS_GET_SOCKET_OPTIONS 88
#define SYS_DEVICE_MANAGEMENT 89
#define SYS_INTERRUPT_THREAD 90
#define SYS_LOAD_KERNEL_MODULE 91
#define SYS_UNLOAD_KERNEL_MODULE 92
#define SYS_FORK 93
#define SYS_GETGID 94
#define SYS_GETEGID 95
#define SYS_GETPPID 96
#define SYS_PIPE 97
#define SYS_GETENTROPY 98
#define SYS_SOCKETPAIR 99
#define SYS_PEERNAME 100
#define SYS_SOCKNAME 101
#define SYS_SIGNAL_ACTION 102
#define SYS_SIGPROCMASK 103
#define SYS_KILL 104
#define SYS_SIGNAL_RETURN 105
#define SYS_ALARM 106
#define SYS_GET_RESOURCE_LIMIT 107 
#define SYS_EPOLL_CREATE 108
#define SYS_EPOLL_CTL 109
#define SYS_EPOLL_WAIT 110
#define SYS_FCHDIR 111

#ifdef __cplusplus
extern "C"{
#endif

__attribute__((__always_inline__))
static inline long syscalln0(uint64_t call) {
    volatile long ret;
    asm volatile("int $0x69" : "=a"(ret) : "a"(call)); 
    return ret;
}

__attribute__((__always_inline__))
static long syscalln1(uint64_t call, uint64_t arg0) {
    volatile long ret;
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "D"(arg0) : "memory"); 
    return ret;
}

__attribute__((__always_inline__))
static long syscalln2(uint64_t call, uint64_t arg0, uint64_t arg1) {
    volatile long ret;
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1) : "memory"); 
    return ret;
}

__attribute__((__always_inline__))
static long syscalln3(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    volatile long ret;
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2) : "memory"); 
    return ret;
}

__attribute__((__always_inline__))
static long syscalln4(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    volatile long ret;
	register uint64_t arg3r asm("r10") = arg3; /* put arg3 in r10 */
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r) : "memory"); 
    return ret;
}

__attribute__((__always_inline__))
static long syscalln5(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    volatile long ret;
	register uint64_t arg3r asm("r10") = arg3; /* put arg3 in r10 */
	register uint64_t arg4r asm("r9") = arg4; /* put arg4 in r9 */
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r) : "memory"); 
    return ret;
}

__attribute__((__always_inline__))
static long syscalln6(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    volatile long ret;
	register uint64_t arg3r asm("r10") = arg3; /* put arg3 in r10 */
	register uint64_t arg4r asm("r9") = arg4; /* put arg4 in r9 */
	register uint64_t arg5r asm("r8") = arg5; /* put arg5 in r8 */
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r), "r"(arg5r) : "memory"); 
    return ret;
}

#ifdef __cplusplus
}
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call) { return syscalln0(call); }
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0) { return syscalln1(call, arg0); }
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1) { return syscalln2(call, arg0, arg1); }
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) { return syscalln3(call, arg0, arg1, arg2); }
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) { return syscalln4(call, arg0, arg1, arg2, arg3); }
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)  { return syscalln5(call, arg0, arg1, arg2, arg3, arg4); }
    __attribute__((__always_inline__)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)  { return syscalln6(call, arg0, arg1, arg2, arg3, arg4, arg5); }

    template<typename... T>
    __attribute__((__always_inline__)) static inline long syscall(uint64_t call, T... args){
        return _syscall(call, (uint64_t)(args)...);
    }
#else
    #define GET_SYSCALL(a0, a1, a2, a3, a4, a5, a6, name, ...) name
    #define syscall(...) GET_SYSCALL(__VA_ARGS__, syscalln6, syscalln5, syscalln4, syscalln3, syscalln2, syscalln1, syscalln0)(__VA_ARGS__) 
#endif

#endif

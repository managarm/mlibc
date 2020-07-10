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
#define SYS_CHDIR 12
#define SYS_TIME 13
#define SYS_MAP_FB 14
#define SYS_ALLOC 15
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
#define SYS_GRANT_PTY 36
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

static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    volatile long ret;
    asm volatile("int $0x69" : "=a"(ret) : "a"(call), "b"(arg0), "c"(arg1), "d"(arg2), "S"(arg3), "D"(arg4) : "memory"); 
    return ret;
}

#define syscall(call, arg0, arg1, arg2, arg3, arg4) _syscall(call, (uint64_t)arg0, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3, (uint64_t)arg4)

#endif
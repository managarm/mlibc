#include <cstddef>
#include <cstdint>

#define SyscallInvoker "syscall"

#define STUB(signature)                                                                            \
	signature { sys_libc_log(#signature "is a stub!"); }
#define STUB_RET(signature)                                                                        \
	signature {                                                                                    \
		sys_libc_log(#signature " is a stub!");                                                    \
		return 0;                                                                                  \
	}

inline int syscall_error(intptr_t ret) {
	auto v = static_cast<intptr_t>(ret);
	if (static_cast<uintptr_t>(v) > -4096ul)
		return -v;
	return 0;
}

#pragma region
constexpr size_t SYS_READ = 0;
constexpr size_t SYS_WRITE = 1;
constexpr size_t SYS_OPEN = 2;
constexpr size_t SYS_CLOSE = 3;
constexpr size_t SYS_STAT = 4;
constexpr size_t SYS_FSTAT = 5;
constexpr size_t SYS_LSTAT = 6;
constexpr size_t SYS_LSEEK = 8;
constexpr size_t SYS_MMAP = 9;
constexpr size_t SYS_MPROTECT = 10;
constexpr size_t SYS_MUNMAP = 11;
constexpr size_t SYS_IOCTL = 16;
constexpr size_t SYS_ACCESS = 21;
constexpr size_t SYS_PIPE = 22;
constexpr size_t SYS_DUP = 32;
constexpr size_t SYS_DUP2 = 33;
constexpr size_t SYS_NANOSLEEP = 35;
constexpr size_t SYS_GETPID = 39;
constexpr size_t SYS_FORK = 57;
constexpr size_t SYS_EXECVE = 59;
constexpr size_t SYS_EXIT = 60;
constexpr size_t SYS_WAIT4 = 61;
constexpr size_t SYS_UNAME = 63;
constexpr size_t SYS_FCNTL = 72;
constexpr size_t SYS_GETCWD = 79;
constexpr size_t SYS_CHDIR = 80;
constexpr size_t SYS_FCHDIR = 81;
constexpr size_t SYS_RENAME = 82;
constexpr size_t SYS_MKDIR = 83;
constexpr size_t SYS_CREAT = 85;
constexpr size_t SYS_LINK = 86;
constexpr size_t SYS_READLINK = 89;
constexpr size_t SYS_UMASK = 95;
constexpr size_t SYS_GETRLIMIT = 97;
constexpr size_t SYS_GETRUSAGE = 98;
constexpr size_t SYS_GETUID = 102;
constexpr size_t SYS_GETGID = 104;
constexpr size_t SYS_GETEUID = 107;
constexpr size_t SYS_GETEGID = 108;
constexpr size_t SYS_SETPGID = 109;
constexpr size_t SYS_GETPPID = 110;
constexpr size_t SYS_SETSID = 112;
constexpr size_t SYS_GETPGID = 121;
constexpr size_t SYS_GETSID = 124;
constexpr size_t SYS_STATFS = 137;
constexpr size_t SYS_ARCH_PRCTL = 158;
constexpr size_t SYS_MOUNT = 165;
constexpr size_t SYS_UMOUNT = 166;
constexpr size_t SYS_REBOOT = 169;
constexpr size_t SYS_GETDENTS64 = 217;
constexpr size_t SYS_CLOCK_GETTIME = 228;
constexpr size_t SYS_PANIC = 255;
constexpr size_t SYS_OPENAT = 257;
constexpr size_t SYS_MKDIRAT = 258;
constexpr size_t SYS_FSTATAT = 262;
constexpr size_t SYS_RENAMEAT = 264;
constexpr size_t SYS_LINKAT = 265;
constexpr size_t SYS_READLINKAT = 267;
constexpr size_t SYS_FCHMODAT = 268;
constexpr size_t SYS_UTIMENSAT = 280;

#pragma endregion

static inline uintptr_t SyscallAsm0(uintptr_t n) {

	uintptr_t ret;
	asm volatile(SyscallInvoker : "=a"(ret) : "a"(n) : "rcx", "r11", "memory");
	return ret;
}

static inline uintptr_t SyscallAsm1(uintptr_t n, uintptr_t a1) {
	uintptr_t ret;
	asm volatile(SyscallInvoker : "=a"(ret) : "a"(n), "D"(a1) : "rcx", "r11", "memory");
	return ret;
}

static inline uintptr_t SyscallAsm2(uintptr_t n, uintptr_t a1, uintptr_t a2) {
	uintptr_t ret;
	asm volatile(SyscallInvoker : "=a"(ret) : "a"(n), "D"(a1), "S"(a2) : "rcx", "r11", "memory");
	return ret;
}

static inline uintptr_t SyscallAsm3(uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3) {
	uintptr_t ret;
	asm volatile(SyscallInvoker
	             : "=a"(ret)
	             : "a"(n), "D"(a1), "S"(a2), "d"(a3)
	             : "rcx", "r11", "memory");
	return ret;
}

static inline uintptr_t
SyscallAsm4(uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4) {
	uintptr_t ret;
	register uintptr_t r10 asm("r10") = a4;
	asm volatile(SyscallInvoker
	             : "=a"(ret)
	             : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10)
	             : "rcx", "r11", "memory");
	return ret;
}

static inline uintptr_t
SyscallAsm5(uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5) {
	uintptr_t ret;
	register uintptr_t r10 asm("r10") = a4;
	register uintptr_t r8 asm("r8") = a5;
	asm volatile(SyscallInvoker
	             : "=a"(ret)
	             : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8)
	             : "rcx", "r11", "memory");
	return ret;
}

static inline uintptr_t SyscallAsm6(
    uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6
) {
	uintptr_t ret;
	register uintptr_t r10 asm("r10") = a4;
	register uintptr_t r8 asm("r8") = a5;
	register uintptr_t r9 asm("r9") = a6;
	asm volatile(SyscallInvoker
	             : "=a"(ret)
	             : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8), "r"(r9)
	             : "rcx", "r11", "memory");
	return ret;
}

#define GetMacro(_1, _2, _3, _4, _5, _6, _7, NAME, ...) NAME
#define InvokeSyscall(...)                                                                                            \
	GetMacro(__VA_ARGS__, SyscallAsm6, SyscallAsm5, SyscallAsm4, SyscallAsm3, SyscallAsm2, SyscallAsm1, SyscallAsm0)( \
	    __VA_ARGS__                                                                                                   \
	)

inline uintptr_t DoSyscall(
    uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6
) {
	return InvokeSyscall(n, a1, a2, a3, a4, a5, a6);
}
inline uintptr_t
DoSyscall(uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5) {
	return InvokeSyscall(n, a1, a2, a3, a4, a5);
}
inline uintptr_t DoSyscall(uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4) {
	return InvokeSyscall(n, a1, a2, a3, a4);
}
inline uintptr_t DoSyscall(uintptr_t n, uintptr_t a1, uintptr_t a2, uintptr_t a3) {
	return InvokeSyscall(n, a1, a2, a3);
}
inline uintptr_t DoSyscall(uintptr_t n, uintptr_t a1, uintptr_t a2) {
	return InvokeSyscall(n, a1, a2);
}
inline uintptr_t DoSyscall(uintptr_t n, uintptr_t a1) { return InvokeSyscall(n, a1); }
inline uintptr_t DoSyscall(uintptr_t n) { return InvokeSyscall(n); }

template <typename... Args>
inline uintptr_t Syscall(Args &&...args) {
	return DoSyscall(uintptr_t(args)...);
}

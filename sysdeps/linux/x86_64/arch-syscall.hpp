#pragma once

// GCC allows register + asm placement in extern "C" mode, but not in C++ mode.
extern "C" {
	using sc_word_t = long;

	static sc_word_t do_asm_syscall0(int sc) {
		sc_word_t ret;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc)
				: "rcx", "r11", "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall1(int sc,
			sc_word_t arg1) {
		sc_word_t ret;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc), "D"(arg1)
				: "rcx", "r11", "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall2(int sc,
			sc_word_t arg1, sc_word_t arg2) {
		sc_word_t ret;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc), "D"(arg1), "S"(arg2)
				: "rcx", "r11", "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall3(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
		sc_word_t ret;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3)
				: "rcx", "r11", "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall4(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4) {
		sc_word_t ret;
		register sc_word_t arg4_reg asm("r10") = arg4;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3),
					"r"(arg4_reg)
				: "rcx", "r11", "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall5(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5) {
		sc_word_t ret;
		register sc_word_t arg4_reg asm("r10") = arg4;
		register sc_word_t arg5_reg asm("r8") = arg5;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3),
					"r"(arg4_reg), "r"(arg5_reg)
				: "rcx", "r11", "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall6(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
		sc_word_t ret;
		register sc_word_t arg4_reg asm("r10") = arg4;
		register sc_word_t arg5_reg asm("r8") = arg5;
		register sc_word_t arg6_reg asm("r9") = arg6;
		asm volatile ("syscall" : "=a"(ret)
				: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3),
					"r"(arg4_reg), "r"(arg5_reg), "r"(arg6_reg)
				: "rcx", "r11", "memory");
		return ret;
	}
}

#define NR_read 0
#define NR_write 1
#define NR_open 2
#define NR_close 3
#define NR_stat 4
#define NR_fstat 5
#define NR_lseek 8
#define NR_mmap 9
#define NR_mprotect 10
#define NR_rt_sigaction 13
#define NR_rt_sigprocmask 14
#define NR_ioctl 16
#define NR_access 21
#define NR_pipe 22
#define NR_select 23
#define NR_nanosleep 35
#define NR_getpid 39
#define NR_socket 41
#define NR_connect 42
#define NR_accept 43
#define NR_sendmsg 46
#define NR_recvmsg 47
#define NR_shutdown 48
#define NR_bind 49
#define NR_listen 50
#define NR_getsockname 51
#define NR_getpeername 52
#define NR_socketpair 53
#define NR_setsockopt 54
#define NR_getsockopt 55
#define NR_clone 56
#define NR_fork 57
#define NR_execve 59
#define NR_exit 60
#define NR_wait4 61
#define NR_kill 62
#define NR_fcntl 72
#define NR_getcwd 79
#define NR_chdir 80
#define NR_mkdir 83
#define NR_rmdir 84
#define NR_unlink 87
#define NR_symlink 88
#define NR_readlink 89
#define NR_getrlimit 97
#define NR_getuid 102
#define NR_getgid 104
#define NR_geteuid 107
#define NR_getegid 108
#define NR_rt_sigsuspend 130
#define NR_sigaltstack 131
#define NR_arch_prctl 158
#define NR_setrlimit 160
#define NR_sys_futex 202
#define NR_clock_gettime 228
#define NR_exit_group 231
#define NR_tgkill 234
#define NR_mkdirat 258
#define NR_newfstatat 262
#define NR_unlinkat 263
#define NR_pselect6 270
#define NR_accept4 288
#define NR_dup3 292
#define NR_pipe2 293

#define ARCH_SET_FS	0x1002

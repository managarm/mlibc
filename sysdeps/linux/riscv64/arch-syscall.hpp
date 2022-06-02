#pragma once

// GCC allows register + asm placement in extern "C" mode, but not in C++ mode.
extern "C" {
	using sc_word_t = long;

	static sc_word_t do_asm_syscall0(int sc) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) : "r"(sc_reg) : "memory", "a1");
		return ret;
	}

	static sc_word_t do_asm_syscall1(int sc,
			sc_word_t arg1) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t arg1_reg asm("a0") = arg1;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) :
				"r"(sc_reg),
				"r"(arg1_reg)
				: "memory", "a1");
		return ret;
	}

	static sc_word_t do_asm_syscall2(int sc,
			sc_word_t arg1, sc_word_t arg2) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t arg1_reg asm("a0") = arg1;
		register sc_word_t arg2_reg asm("a1") = arg2;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) :
				"r"(sc_reg),
				"r"(arg1_reg),
				"r"(arg2_reg)
				: "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall3(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t arg1_reg asm("a0") = arg1;
		register sc_word_t arg2_reg asm("a1") = arg2;
		register sc_word_t arg3_reg asm("a2") = arg3;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) :
				"r"(sc_reg),
				"r"(arg1_reg),
				"r"(arg2_reg),
				"r"(arg3_reg)
				: "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall4(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t arg1_reg asm("a0") = arg1;
		register sc_word_t arg2_reg asm("a1") = arg2;
		register sc_word_t arg3_reg asm("a2") = arg3;
		register sc_word_t arg4_reg asm("a3") = arg4;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) :
				"r"(sc_reg),
				"r"(arg1_reg),
				"r"(arg2_reg),
				"r"(arg3_reg),
				"r"(arg4_reg)
				: "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall5(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t arg1_reg asm("a0") = arg1;
		register sc_word_t arg2_reg asm("a1") = arg2;
		register sc_word_t arg3_reg asm("a2") = arg3;
		register sc_word_t arg4_reg asm("a3") = arg4;
		register sc_word_t arg5_reg asm("a4") = arg5;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) :
				"r"(sc_reg),
				"r"(arg1_reg),
				"r"(arg2_reg),
				"r"(arg3_reg),
				"r"(arg4_reg),
				"r"(arg5_reg)
				: "memory");
		return ret;
	}

	static sc_word_t do_asm_syscall6(int sc,
			sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
			sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
		register int sc_reg asm("a7") = sc;
		register sc_word_t arg1_reg asm("a0") = arg1;
		register sc_word_t arg2_reg asm("a1") = arg2;
		register sc_word_t arg3_reg asm("a2") = arg3;
		register sc_word_t arg4_reg asm("a3") = arg4;
		register sc_word_t arg5_reg asm("a4") = arg5;
		register sc_word_t arg6_reg asm("a5") = arg6;
		register sc_word_t ret asm("a0");
		asm volatile ("ecall" : "=r"(ret) :
				"r"(sc_reg),
				"r"(arg1_reg),
				"r"(arg2_reg),
				"r"(arg3_reg),
				"r"(arg4_reg),
				"r"(arg5_reg),
				"r"(arg6_reg)
				: "memory"
				);
		return ret;
	}
}

#define NR_sys_futex NR_futex

#define NR_io_setup 0
#define NR_io_destroy 1
#define NR_io_submit 2
#define NR_io_cancel 3
#define NR_io_getevents 4
#define NR_setxattr 5
#define NR_lsetxattr 6
#define NR_fsetxattr 7
#define NR_getxattr 8
#define NR_lgetxattr 9
#define NR_fgetxattr 10
#define NR_listxattr 11
#define NR_llistxattr 12
#define NR_flistxattr 13
#define NR_removexattr 14
#define NR_lremovexattr 15
#define NR_fremovexattr 16
#define NR_getcwd 17
#define NR_lookup_dcookie 18
#define NR_eventfd2 19
#define NR_epoll_create1 20
#define NR_epoll_ctl 21
#define NR_epoll_pwait 22
#define NR_dup 23
#define NR_dup3 24
#define NR3264_fcntl 25
#define NR_inotify_init1 26
#define NR_inotify_add_watch 27
#define NR_inotify_rm_watch 28
#define NR_ioctl 29
#define NR_ioprio_set 30
#define NR_ioprio_get 31
#define NR_flock 32
#define NR_mknodat 33
#define NR_mkdirat 34
#define NR_unlinkat 35
#define NR_symlinkat 36
#define NR_linkat 37
#define NR_renameat 38
#define NR_umount2 39
#define NR_mount 40
#define NR_pivot_root 41
#define NR_nfsservctl 42
#define NR3264_statfs 43
#define NR3264_fstatfs 44
#define NR3264_truncate 45
#define NR3264_ftruncate 46
#define NR_fallocate 47
#define NR_faccessat 48
#define NR_chdir 49
#define NR_fchdir 50
#define NR_chroot 51
#define NR_fchmod 52
#define NR_fchmodat 53
#define NR_fchownat 54
#define NR_fchown 55
#define NR_openat 56
#define NR_close 57
#define NR_vhangup 58
#define NR_pipe2 59
#define NR_quotactl 60
#define NR_getdents64 61
#define NR3264_lseek 62
#define NR_read 63
#define NR_write 64
#define NR_readv 65
#define NR_writev 66
#define NR_pread64 67
#define NR_pwrite64 68
#define NR_preadv 69
#define NR_pwritev 70
#define NR3264_sendfile 71
#define NR_pselect6 72
#define NR_ppoll 73
#define NR_signalfd4 74
#define NR_vmsplice 75
#define NR_splice 76
#define NR_tee 77
#define NR_readlinkat 78
#define NR3264_fstatat 79
#define NR3264_fstat 80
#define NR_sync 81
#define NR_fsync 82
#define NR_fdatasync 83
#define NR_sync_file_range2 84
#define NR_sync_file_range 84
#define NR_timerfd_create 85
#define NR_timerfd_settime 86
#define NR_timerfd_gettime 87
#define NR_utimensat 88
#define NR_acct 89
#define NR_capget 90
#define NR_capset 91
#define NR_personality 92
#define NR_exit 93
#define NR_exit_group 94
#define NR_waitid 95
#define NR_set_tid_address 96
#define NR_unshare 97
#define NR_futex 98
#define NR_set_robust_list 99
#define NR_get_robust_list 100
#define NR_nanosleep 101
#define NR_getitimer 102
#define NR_setitimer 103
#define NR_kexec_load 104
#define NR_init_module 105
#define NR_delete_module 106
#define NR_timer_create 107
#define NR_timer_gettime 108
#define NR_timer_getoverrun 109
#define NR_timer_settime 110
#define NR_timer_delete 111
#define NR_clock_settime 112
#define NR_clock_gettime 113
#define NR_clock_getres 114
#define NR_clock_nanosleep 115
#define NR_syslog 116
#define NR_ptrace 117
#define NR_sched_setparam 118
#define NR_sched_setscheduler 119
#define NR_sched_getscheduler 120
#define NR_sched_getparam 121
#define NR_sched_setaffinity 122
#define NR_sched_getaffinity 123
#define NR_sched_yield 124
#define NR_sched_get_priority_max 125
#define NR_sched_get_priority_min 126
#define NR_sched_rr_get_interval 127
#define NR_restart_syscall 128
#define NR_kill 129
#define NR_tkill 130
#define NR_tgkill 131
#define NR_sigaltstack 132
#define NR_rt_sigsuspend 133
#define NR_rt_sigaction 134
#define NR_rt_sigprocmask 135
#define NR_rt_sigpending 136
#define NR_rt_sigtimedwait 137
#define NR_rt_sigqueueinfo 138
#define NR_rt_sigreturn 139
#define NR_setpriority 140
#define NR_getpriority 141
#define NR_reboot 142
#define NR_setregid 143
#define NR_setgid 144
#define NR_setreuid 145
#define NR_setuid 146
#define NR_setresuid 147
#define NR_getresuid 148
#define NR_setresgid 149
#define NR_getresgid 150
#define NR_setfsuid 151
#define NR_setfsgid 152
#define NR_times 153
#define NR_setpgid 154
#define NR_getpgid 155
#define NR_getsid 156
#define NR_setsid 157
#define NR_getgroups 158
#define NR_setgroups 159
#define NR_uname 160
#define NR_sethostname 161
#define NR_setdomainname 162
#define NR_getrlimit 163
#define NR_setrlimit 164
#define NR_getrusage 165
#define NR_umask 166
#define NR_prctl 167
#define NR_getcpu 168
#define NR_gettimeofday 169
#define NR_settimeofday 170
#define NR_adjtimex 171
#define NR_getpid 172
#define NR_getppid 173
#define NR_getuid 174
#define NR_geteuid 175
#define NR_getgid 176
#define NR_getegid 177
#define NR_gettid 178
#define NR_sysinfo 179
#define NR_mq_open 180
#define NR_mq_unlink 181
#define NR_mq_timedsend 182
#define NR_mq_timedreceive 183
#define NR_mq_notify 184
#define NR_mq_getsetattr 185
#define NR_msgget 186
#define NR_msgctl 187
#define NR_msgrcv 188
#define NR_msgsnd 189
#define NR_semget 190
#define NR_semctl 191
#define NR_semtimedop 192
#define NR_semop 193
#define NR_shmget 194
#define NR_shmctl 195
#define NR_shmat 196
#define NR_shmdt 197
#define NR_socket 198
#define NR_socketpair 199
#define NR_bind 200
#define NR_listen 201
#define NR_accept 202
#define NR_connect 203
#define NR_getsockname 204
#define NR_getpeername 205
#define NR_sendto 206
#define NR_recvfrom 207
#define NR_setsockopt 208
#define NR_getsockopt 209
#define NR_shutdown 210
#define NR_sendmsg 211
#define NR_recvmsg 212
#define NR_readahead 213
#define NR_brk 214
#define NR_munmap 215
#define NR_mremap 216
#define NR_add_key 217
#define NR_request_key 218
#define NR_keyctl 219
#define NR_clone 220
#define NR_execve 221
#define NR3264_mmap 222
#define NR3264_fadvise64 223
#define NR_swapon 224
#define NR_swapoff 225
#define NR_mprotect 226
#define NR_msync 227
#define NR_mlock 228
#define NR_munlock 229
#define NR_mlockall 230
#define NR_munlockall 231
#define NR_mincore 232
#define NR_madvise 233
#define NR_remap_file_pages 234
#define NR_mbind 235
#define NR_get_mempolicy 236
#define NR_set_mempolicy 237
#define NR_migrate_pages 238
#define NR_move_pages 239
#define NR_rt_tgsigqueueinfo 240
#define NR_perf_event_open 241
#define NR_accept4 242
#define NR_recvmmsg 243
#define NR_arch_specific_syscall 244
#define NR_wait4 260
#define NR_prlimit64 261
#define NR_fanotify_init 262
#define NR_fanotify_mark 263
#define NR_name_to_handle_at         264
#define NR_open_by_handle_at         265
#define NR_clock_adjtime 266
#define NR_syncfs 267
#define NR_setns 268
#define NR_sendmmsg 269
#define NR_process_vm_readv 270
#define NR_process_vm_writev 271
#define NR_kcmp 272
#define NR_finit_module 273
#define NR_sched_setattr 274
#define NR_sched_getattr 275
#define NR_renameat2 276
#define NR_seccomp 277
#define NR_getrandom 278
#define NR_memfd_create 279
#define NR_bpf 280
#define NR_execveat 281
#define NR_userfaultfd 282
#define NR_membarrier 283
#define NR_mlock2 284
#define NR_copy_file_range 285
#define NR_preadv2 286
#define NR_pwritev2 287
#define NR_pkey_mprotect 288
#define NR_pkey_alloc 289
#define NR_pkey_free 290
#define NR_statx 291
#define NR_io_pgetevents 292
#define NR_rseq 293
#define NR_kexec_file_load 294
#define NR_clock_gettime64 403
#define NR_clock_settime64 404
#define NR_clock_adjtime64 405
#define NR_clock_getres_time64 406
#define NR_clock_nanosleep_time64 407
#define NR_timer_gettime64 408
#define NR_timer_settime64 409
#define NR_timerfd_gettime64 410
#define NR_timerfd_settime64 411
#define NR_utimensat_time64 412
#define NR_pselect6_time64 413
#define NR_ppoll_time64 414
#define NR_io_pgetevents_time64 416
#define NR_recvmmsg_time64 417
#define NR_mq_timedsend_time64 418
#define NR_mq_timedreceive_time64 419
#define NR_semtimedop_time64 420
#define NR_rt_sigtimedwait_time64 421
#define NR_futex_time64 422
#define NR_sched_rr_get_interval_time64 423
#define NR_pidfd_send_signal 424
#define NR_io_uring_setup 425
#define NR_io_uring_enter 426
#define NR_io_uring_register 427
#define NR_open_tree 428
#define NR_move_mount 429
#define NR_fsopen 430
#define NR_fsconfig 431
#define NR_fsmount 432
#define NR_fspick 433
#define NR_pidfd_open 434
#define NR_clone3 435
#define NR_close_range 436
#define NR_openat2 437
#define NR_pidfd_getfd 438
#define NR_faccessat2 439
#define NR_process_madvise 440
#define NR_epoll_pwait2 441
#define NR_mount_setattr 442
#define NR_quotactl_fd 443
#define NR_landlock_create_ruleset 444
#define NR_landlock_add_rule 445
#define NR_landlock_restrict_self 446
#define NR_memfd_secret 447
#define NR_process_mrelease 448
#define NR_futex_waitv 449
#define NR_set_mempolicy_home_node 450
#define NR_syscalls 451
#define NR_fcntl NR3264_fcntl
#define NR_statfs NR3264_statfs
#define NR_fstatfs NR3264_fstatfs
#define NR_truncate NR3264_truncate
#define NR_ftruncate NR3264_ftruncate
#define NR_lseek NR3264_lseek
#define NR_sendfile NR3264_sendfile
#define NR_newfstatat NR3264_fstatat
#define NR_fstat NR3264_fstat
#define NR_mmap NR3264_mmap
#define NR_fadvise64 NR3264_fadvise64
#define NR_stat NR3264_stat
#define NR_lstat NR3264_lstat
#define NR_fcntl64 NR3264_fcntl
#define NR_statfs64 NR3264_statfs
#define NR_fstatfs64 NR3264_fstatfs
#define NR_truncate64 NR3264_truncate
#define NR_ftruncate64 NR3264_ftruncate
#define NR_llseek NR3264_lseek
#define NR_sendfile64 NR3264_sendfile
#define NR_fstatat64 NR3264_fstatat
#define NR_fstat64 NR3264_fstat
#define NR_mmap2 NR3264_mmap
#define NR_fadvise64_64 NR3264_fadvise64
#define NR_stat64 NR3264_stat
#define NR_lstat64 NR3264_lstat

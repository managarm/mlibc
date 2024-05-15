
#ifndef _UNISTD_H
#define _UNISTD_H

#include <mlibc-config.h>
#include <bits/types.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/off_t.h>
#include <bits/types.h>
#include <abi-bits/access.h>
#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/seek-whence.h>

#if __MLIBC_SYSDEP_HAS_BITS_SYSCALL_H && __MLIBC_LINUX_OPTION
#include <bits/syscall.h>
#endif /* __MLIBC_SYSDEP_HAS_BITS_SYSCALL_H && __MLIBC_LINUX_OPTION */

#ifdef __cplusplus
extern "C" {
#endif

#define _POSIX_VERSION 200809L
#define _POSIX2_VERSION _POSIX_VERSION
#define _XOPEN_VERSION 700

#define _POSIX_FSYNC _POSIX_VERSION
#define _POSIX_IPV6 _POSIX_VERSION
#define _POSIX_JOB_CONTROL 1
#define _POSIX_SAVED_IDS 1
#define _POSIX_SHELL 1
#define _POSIX_SPAWN _POSIX_VERSION
#define _POSIX_THREADS _POSIX_VERSION
#define _POSIX_THREAD_SAFE_FUNCTIONS _POSIX_VERSION
#define _POSIX_MONOTONIC_CLOCK 0

#if __MLIBC_CRYPT_OPTION
#define _XOPEN_CRYPT 1
#endif

// MISSING: additional _POSIX and _XOPEN feature macros
// MISSING: _POSIX_TIMESTAMP_RESOLUTION and _POSIX2_SYMLINKS

#define _CS_PATH 0
#define _CS_POSIX_V6_WIDTH_RESTRICTED_ENVS 1
#define _CS_GNU_LIBC_VERSION 2
#define _CS_GNU_LIBPTHREAD_VERSION 3
#define _CS_POSIX_V5_WIDTH_RESTRICTED_ENVS 4
#define _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS 5

#define _CS_POSIX_V6_ILP32_OFF32_CFLAGS 1116
#define _CS_POSIX_V6_ILP32_OFF32_LDFLAGS 1117
#define _CS_POSIX_V6_ILP32_OFF32_LIBS 1118
#define _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS 1119
#define _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS 1120
#define _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS 1121
#define _CS_POSIX_V6_ILP32_OFFBIG_LIBS 1122
#define _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS 1123
#define _CS_POSIX_V6_LP64_OFF64_CFLAGS 1124
#define _CS_POSIX_V6_LP64_OFF64_LDFLAGS 1125
#define _CS_POSIX_V6_LP64_OFF64_LIBS 1126
#define _CS_POSIX_V6_LP64_OFF64_LINTFLAGS 1127
#define _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS 1128
#define _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS 1129
#define _CS_POSIX_V6_LPBIG_OFFBIG_LIBS 1130
#define _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS 1131
#define _CS_POSIX_V7_ILP32_OFF32_CFLAGS 1132
#define _CS_POSIX_V7_ILP32_OFF32_LDFLAGS 1133
#define _CS_POSIX_V7_ILP32_OFF32_LIBS 1134
#define _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS 1135
#define _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS 1136
#define _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS 1137
#define _CS_POSIX_V7_ILP32_OFFBIG_LIBS 1138
#define _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS 1139
#define _CS_POSIX_V7_LP64_OFF64_CFLAGS 1140
#define _CS_POSIX_V7_LP64_OFF64_LDFLAGS 1141
#define _CS_POSIX_V7_LP64_OFF64_LIBS 1142
#define _CS_POSIX_V7_LP64_OFF64_LINTFLAGS 1143
#define _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS 1144
#define _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS 1145
#define _CS_POSIX_V7_LPBIG_OFFBIG_LIBS 1146
#define _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS 1147
#define _CS_V6_ENV 1148
#define _CS_V7_ENV 1149

// MISSING: SEEK macros from stdio.h

#define F_LOCK 1
#define F_TEST 2
#define F_TLOCK 3
#define F_ULOCK 4

// MISSING: _PC macros
// For now, use the Linux ABI for _PC constants.
#define _PC_LINK_MAX		0
#define _PC_MAX_CANON		1
#define _PC_MAX_INPUT		2
#define _PC_NAME_MAX		3
#define _PC_PATH_MAX		4
#define _PC_PIPE_BUF		5
#define _PC_CHOWN_RESTRICTED	6
#define _PC_NO_TRUNC		7
#define _PC_VDISABLE		8

#define _PC_FILESIZEBITS 9
#define _PC_SYMLINK_MAX 10

// MISSING: remaining _SC_macros
#define _SC_ARG_MAX 0
#define _SC_CHILD_MAX 1
#define _SC_CLK_TCK 2
#define _SC_NGROUPS_MAX 3
#define _SC_OPEN_MAX 4
#define _SC_STREAM_MAX 5
#define _SC_TZNAME_MAX 6
#define _SC_JOB_CONTROL 7

#define _SC_SAVED_IDS 8
#define _SC_REALTIME_SIGNALS 9
#define _SC_PRIORITY_SCHEDULING 10
#define _SC_TIMERS 11
#define _SC_ASYNCHRONOUS_IO 12
#define _SC_PRIORITIZED_IO 13
#define _SC_SYNCHRONIZED_IO 14
#define _SC_FSYNC 15
#define _SC_MAPPED_FILES 16
#define _SC_MEMLOCK 17
#define _SC_MEMLOCK_RANGE 18
#define _SC_MEMORY_PROTECTION 19
#define _SC_MESSAGE_PASSING 20
#define _SC_SEMAPHORES 21
#define _SC_SHARED_MEMORY_OBJECTS 22
#define _SC_AIO_LISTIO_MAX 23
#define _SC_AIO_MAX 24
#define _SC_AIO_PRIO_DELTA_MAX 25
#define _SC_DELAYTIMER_MAX 26
#define _SC_MQ_OPEN_MAX 27
#define _SC_MQ_PRIO_MAX 28
#define _SC_VERSION 29
#define _SC_PAGE_SIZE 30
#define _SC_PAGESIZE _SC_PAGE_SIZE
#define _SC_RTSIG_MAX 31
#define _SC_SEM_NSEMS_MAX 32
#define _SC_SEM_VALUE_MAX 33
#define _SC_SIGQUEUE_MAX 34
#define _SC_TIMER_MAX 35
#define _SC_BC_BASE_MAX 36
#define _SC_BC_DIM_MAX 37
#define _SC_BC_SCALE_MAX 38
#define _SC_BC_STRING_MAX 39
#define _SC_COLL_WEIGHTS_MAX 40
#define _SC_EXPR_NEST_MAX 42
#define _SC_LINE_MAX 43
#define _SC_RE_DUP_MAX 44
#define _SC_2_VERSION 46
#define _SC_2_C_BIND 47
#define _SC_2_C_DEV 48
#define _SC_2_FORT_DEV 49
#define _SC_2_FORT_RUN 50
#define _SC_2_SW_DEV 51
#define _SC_2_LOCALEDEF 52
#define _SC_IOV_MAX 60
#define _SC_UIO_MAXIOV _SC_IOV_MAX
#define _SC_THREADS 67
#define _SC_THREAD_SAFE_FUNCTIONS 68
#define _SC_GETGR_R_SIZE_MAX 69
#define _SC_GETPW_R_SIZE_MAX 70
#define _SC_LOGIN_NAME_MAX 71
#define _SC_TTY_NAME_MAX 72
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 73
#define _SC_THREAD_KEYS_MAX 74
#define _SC_THREAD_STACK_MIN 75
#define _SC_THREAD_THREADS_MAX 76
#define _SC_THREAD_ATTR_STACKADDR 77
#define _SC_THREAD_ATTR_STACKSIZE 78
#define _SC_THREAD_PRIORITY_SCHEDULING 79
#define _SC_THREAD_PRIO_INHERIT 80
#define _SC_THREAD_PRIO_PROTECT 81
#define _SC_THREAD_PROCESS_SHARED 82
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PHYS_PAGES 85
#define _SC_AVPHYS_PAGES 86
#define _SC_ATEXIT_MAX 87
#define _SC_PASS_MAX 88
#define _SC_XOPEN_VERSION 89
#define _SC_XOPEN_XCU_VERSION 90
#define _SC_XOPEN_UNIX 91
#define _SC_XOPEN_CRYPT 92
#define _SC_XOPEN_ENH_I18N 93
#define _SC_XOPEN_SHM 94
#define _SC_2_CHAR_TERM 95
#define _SC_2_UPE 97
#define _SC_XOPEN_XPG2 98
#define _SC_XOPEN_XPG3 99
#define _SC_XOPEN_XPG4 100
#define _SC_NZERO 109
#define _SC_XBS5_ILP32_OFF32 125
#define _SC_XBS5_ILP32_OFFBIG 126
#define _SC_XBS5_LP64_OFF64 127
#define _SC_XBS5_LPBIG_OFFBIG 128
#define _SC_XOPEN_LEGACY 129
#define _SC_XOPEN_REALTIME 130
#define _SC_XOPEN_REALTIME_THREADS 131
#define _SC_ADVISORY_INFO 132
#define _SC_BARRIERS 133
#define _SC_CLOCK_SELECTION 137
#define _SC_CPUTIME 138
#define _SC_THREAD_CPUTIME 139
#define _SC_MONOTONIC_CLOCK 149
#define _SC_READER_WRITER_LOCKS 153
#define _SC_SPIN_LOCKS 154
#define _SC_REGEXP 155
#define _SC_SHELL 157
#define _SC_SPAWN 159
#define _SC_SPORADIC_SERVER 160
#define _SC_THREAD_SPORADIC_SERVER 161
#define _SC_TIMEOUTS 164
#define _SC_TYPED_MEMORY_OBJECTS 165
#define _SC_2_PBS 168
#define _SC_2_PBS_ACCOUNTING 169
#define _SC_2_PBS_LOCATE 170
#define _SC_2_PBS_MESSAGE 171
#define _SC_2_PBS_TRACK 172
#define _SC_SYMLOOP_MAX 173
#define _SC_STREAMS 174
#define _SC_2_PBS_CHECKPOINT 175
#define _SC_V6_ILP32_OFF32 176
#define _SC_V6_ILP32_OFFBIG 177
#define _SC_V6_LP64_OFF64 178
#define _SC_V6_LPBIG_OFFBIG 179
#define _SC_HOST_NAME_MAX 180
#define _SC_TRACE 181
#define _SC_TRACE_EVENT_FILTER 182
#define _SC_TRACE_INHERIT 183
#define _SC_TRACE_LOG 184

#define STDERR_FILENO 2
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define _POSIX_VDISABLE '\0'

#define L_ctermid 20

#ifndef intptr_t
typedef __mlibc_intptr intptr_t;
#endif

#ifndef __MLIBC_ABI_ONLY

int access(const char *path, int mode);
unsigned int alarm(unsigned int seconds);
int chdir(const char *path);
int chown(const char *path, uid_t uid, gid_t gid);
int close(int fd);
ssize_t confstr(int, char *, size_t);
char *ctermid(char *s);
int dup(int fd);
int dup2(int src_fd, int dest_fd);
__attribute__((__noreturn__)) void _exit(int status);
void endusershell(void);
int execl(const char *, const char *, ...);
int execle(const char *, const char *, ...);
int execlp(const char *, const char *, ...);
int execv(const char *, char *const []);
int execve(const char *path, char *const argv[], char *const envp[]);
int execvp(const char *, char *const[]);
int execvpe(const char *path, char *const argv[], char *const envp[]);
int faccessat(int, const char *, int, int);
int fchdir(int fd);
int fchown(int fd, uid_t uid, gid_t gid);
int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flags);
int fdatasync(int);
int fexecve(int, char *const [], char *const []);
pid_t fork(void);
pid_t vfork(void);
long fpathconf(int, int);
int fsync(int);
int ftruncate(int, off_t);
int ftruncate64(int, off64_t);
char *getcwd(char *, size_t);
gid_t getegid(void);
uid_t geteuid(void);
gid_t getgid(void);
int getgroups(int, gid_t []);
long gethostid(void);
int gethostname(char *buffer, size_t max_length);
int sethostname(const char *buffer, size_t max_length);
char *getlogin(void);
int getlogin_r(char *, size_t);
int getopt(int, char *const [], const char *);
char *getpass(const char *);
pid_t getpgid(pid_t);
pid_t getpgrp(void);
pid_t getpid(void);
pid_t getppid(void);
pid_t getsid(pid_t);
uid_t getuid(void);
char *getusershell(void);
int isatty(int fd);
int lchown(const char *path, uid_t uid, gid_t gid);
int link(const char *, const char *);
int linkat(int, const char *, int, const char *, int);
int lockf(int, int, off_t);
off_t lseek(int fd, off_t offset, int whence);
off64_t lseek64(int fd, off64_t offset, int whence);
int nice(int);
long pathconf(const char *, int);
int pause(void);
int pipe(int [2]);
ssize_t pread(int, void *, size_t, off_t);
ssize_t pread64(int, void *, size_t, off_t);
ssize_t pwrite(int, const void *, size_t, off_t);
ssize_t pwrite64(int, const void *, size_t, off_t);
ssize_t read(int fd, void *buffer, size_t size);
ssize_t readlink(const char *__restrict, char *__restrict, size_t);
ssize_t readlinkat(int, const char *__restrict, char *__restrict, size_t);
int rmdir(const char *);
int setegid(gid_t);
int seteuid(uid_t);
int setgid(gid_t);
int setpgid(pid_t, pid_t);
pid_t setpgrp(void);
int setregid(gid_t, gid_t);
int setreuid(uid_t, uid_t);
pid_t setsid(void);
int setuid(uid_t);
void setusershell(void);
unsigned int sleep(unsigned int);
void swab(const void *__restrict, void *__restrict, ssize_t);
int symlink(const char *, const char *);
int symlinkat(const char *, int, const char *);
void sync(void);
long sysconf(int);
pid_t tcgetpgrp(int);
int tcsetpgrp(int, pid_t);
int truncate(const char *, off_t);
int truncate64(const char *, off64_t);
char *ttyname(int);
int ttyname_r(int, char *, size_t);
int unlink(const char *);
int unlinkat(int, const char *, int);
ssize_t write(int fd, const void *buffer, size_t size);

extern char **environ;
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

#endif /* !__MLIBC_ABI_ONLY */

// Non-POSIX functions supported by Linux.
#if UINTPTR_MAX == UINT64_MAX
typedef __mlibc_uint64 useconds_t;
#else
typedef __mlibc_uint32 useconds_t;
#endif

#ifndef __MLIBC_ABI_ONLY

int getpagesize(void);
char *get_current_dir_name(void);
int usleep(useconds_t);
int chroot(const char *);
int daemon(int, int);

// This is a Linux extension
pid_t gettid(void);
int getentropy(void *, size_t);

int pipe2(int *pipefd, int flags);

int setresuid(uid_t ruid, uid_t euid, uid_t suid);
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);

/* Glibc extensions. */
int getdomainname(char *name, size_t len);
int setdomainname(const char *name, size_t len);

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);

// Glibc doesn't provide them by default anymore, lock behind an option
#if __MLIBC_CRYPT_OPTION
char *crypt(const char *, const char *);
void encrypt(char block[64], int flags);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#if __MLIBC_LINUX_OPTION
#	include <bits/linux/linux_unistd.h>
#endif

#if __MLIBC_BSD_OPTION
#	include <bits/bsd/bsd_unistd.h>
#endif

#endif // _UNISTD_H


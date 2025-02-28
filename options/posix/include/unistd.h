
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
#include <abi-bits/sysconf.h>

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

/* MISSING: additional _POSIX and _XOPEN feature macros */
/* MISSING: _POSIX_TIMESTAMP_RESOLUTION and _POSIX2_SYMLINKS */

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

/* MISSING: SEEK macros from stdio.h */

#define F_LOCK 1
#define F_TEST 2
#define F_TLOCK 3
#define F_ULOCK 4

/* MISSING: _PC macros */
/* For now, use the Linux ABI for _PC constants. */
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

#define STDERR_FILENO 2
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define _POSIX_VDISABLE '\0'

#define L_ctermid 20

/*
 * Solving this likely requires us to 'factor out' the typedef into a new
 * header file, or use a mechanism like musl's __NEED_intptr_t.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
typedef __mlibc_intptr intptr_t;
#pragma GCC diagnostic pop

#ifndef __MLIBC_ABI_ONLY

int access(const char *__path, int __mode);
unsigned int alarm(unsigned int __seconds);
int chdir(const char *__path);
int chown(const char *__path, uid_t __uid, gid_t __gid);
int close(int __fd);
ssize_t confstr(int __name, char *__buf, size_t __size);
char *ctermid(char *__s);
int dup(int __fd);
int dup2(int __src_fd, int __dest_fd);
__attribute__((__noreturn__)) void _exit(int __status);
void endusershell(void);
int execl(const char *__path, const char *__arg, ...);
int execle(const char *__path, const char *__arg, ...);
int execlp(const char *__file, const char *__arg, ...);
int execv(const char *__path, char *const __argv[]);
int execve(const char *__path, char *const __argv[], char *const __envp[]);
int execvp(const char *__file, char *const __argv[]);
int execvpe(const char *__path, char *const __argv[], char *const __envp[]);
int faccessat(int __fd, const char *__path, int __mode, int __flags);
int fchdir(int __fd);
int fchown(int __fd, uid_t __uid, gid_t __gid);
int fchownat(int __fd, const char *__path, uid_t __uid, gid_t __gid, int __flags);
int fdatasync(int __fd);
int fexecve(int __fd, char *const __argv[], char *const __envp[]);
pid_t fork(void);
pid_t vfork(void);
long fpathconf(int __fd, int __name);
int fsync(int __fd);
int ftruncate(int __fd, off_t __size);
int ftruncate64(int __fd, off64_t __size);
char *getcwd(char *__buffer, size_t __size);
gid_t getegid(void);
uid_t geteuid(void);
gid_t getgid(void);
int getgroups(int __size, gid_t __list[]);
long gethostid(void);
int gethostname(char *__buffer, size_t __max_length);
int sethostname(const char *__buffer, size_t __max_length);
char *getlogin(void);
int getlogin_r(char *__buffer, size_t __size);
int getopt(int __argc, char *const __argv[], const char *__optstring);
char *getpass(const char *__prompt);
pid_t getpgid(pid_t __pid);
pid_t getpgrp(void);
pid_t getpid(void);
pid_t getppid(void);
pid_t getsid(pid_t __pid);
uid_t getuid(void);
char *getusershell(void);
int isatty(int __fd);
int lchown(const char *__path, uid_t __uid, gid_t __gid);
int link(const char *__oldpath, const char *__newpath);
int linkat(int __olddirfd, const char *__oldpath, int __newdirfd, const char *__newpath, int __flags);
int lockf(int __fd, int __op, off_t __size);
off_t lseek(int __fd, off_t __offset, int __whence);
off64_t lseek64(int __fd, off64_t __offset, int __whence);
int nice(int __increment);
long pathconf(const char *__path, int __name);
int pause(void);
int pipe(int __pipefd[2]);
ssize_t pread(int __fd, void *__buf, size_t __size, off_t __offset);
ssize_t pread64(int __fd, void *__buf, size_t __size, off_t __offset);
ssize_t pwrite(int __fd, const void *__buf, size_t __size, off_t __offset);
ssize_t pwrite64(int __fd, const void *__buf, size_t __size, off_t __offset);
ssize_t read(int fd, void *buffer, size_t size);
ssize_t readlink(const char *__restrict __path, char *__restrict __buf, size_t __size);
ssize_t readlinkat(int __dirfd, const char *__restrict __path, char *__restrict __buf, size_t __size);
int rmdir(const char *__path);
int setegid(gid_t __egid);
int seteuid(uid_t __euid);
int setgid(gid_t __gid);
int setpgid(pid_t __pid, pid_t __pgid);
pid_t setpgrp(void);
int setregid(gid_t __rgid, gid_t __egid);
int setreuid(uid_t __ruid, uid_t __euid);
pid_t setsid(void);
int setuid(uid_t __uid);
void setusershell(void);
unsigned int sleep(unsigned int __seconds);
void swab(const void *__restrict __from, void *__restrict __to, ssize_t __size);
int symlink(const char *__target, const char *__linkpath);
int symlinkat(const char *__target, int __newdirfd, const char *__linkpath);
void sync(void);
long sysconf(int __name);
pid_t tcgetpgrp(int __fd);
int tcsetpgrp(int __fd, pid_t __pgrp);
int truncate(const char *__path, off_t __size);
int truncate64(const char *__path, off64_t __size);
char *ttyname(int __fd);
int ttyname_r(int __fd, char *__buf, size_t __size);
int unlink(const char *__path);
int unlinkat(int __dirfd, const char *__path, int __flags);
ssize_t write(int __fd, const void *__buffer, size_t __size);

extern char **environ;
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

#endif /* !__MLIBC_ABI_ONLY */

/* Non-POSIX functions supported by Linux. */
#if UINTPTR_MAX == UINT64_MAX
typedef __mlibc_uint64 useconds_t;
#else
typedef __mlibc_uint32 useconds_t;
#endif

#ifndef __MLIBC_ABI_ONLY

int getpagesize(void);
char *get_current_dir_name(void);
int usleep(useconds_t __usec);
int chroot(const char *__path);
int daemon(int __nochdir, int __noclose);

/* This is a Linux extension */
pid_t gettid(void);
int getentropy(void *__buffer, size_t __size);

int pipe2(int *__pipefd, int __flags);

int setresuid(uid_t __ruid, uid_t __euid, uid_t __suid);
int setresgid(gid_t __rgid, gid_t __egid, gid_t __sgid);

/* Glibc extensions. */
int getdomainname(char *__name, size_t __len);
int setdomainname(const char *__name, size_t __len);

int getresuid(uid_t *__ruid, uid_t *__euid, uid_t *__suid);
int getresgid(gid_t *__rgid, gid_t *__egid, gid_t *__sgid);

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

#endif /* _UNISTD_H */


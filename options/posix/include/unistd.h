
#ifndef _UNISTD_H
#define _UNISTD_H

#include <bits/feature.h>
#include <bits/types.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/off_t.h>
#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _POSIX_VERSION 200809L
#define _POSIX2_VERSION 200809L
#define _XOPEN_VERSION 700

#define _POSIX_THREADS 200809L

// MISSING: additional _POSIX and _XOPEN feature macros
// MISSING: _POSIX_TIMESTAMP_RESOLUTION and _POSIX2_SYMLINKS

#define F_OK 1
#define R_OK 2
#define W_OK 4
#define X_OK 8

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

// MISSING: remaining _SC_macros
#define _SC_ARG_MAX 0
#define _SC_GETPW_R_SIZE_MAX 1
#define _SC_PHYS_PAGES 2
#define _SC_PAGE_SIZE 3
#define _SC_PAGESIZE _SC_PAGE_SIZE
#define _SC_OPEN_MAX 5
#define _SC_NPROCESSORS_ONLN 6
#define _SC_GETGR_R_SIZE_MAX 7

#define _SC_CHILD_MAX 8
#define _SC_CLK_TCK 9
#define _SC_NGROUPS_MAX 10
#define _SC_VERSION 11
#define _SC_SAVED_IDS 12
#define _SC_JOB_CONTROL 13

#define STDERR_FILENO 2
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define _POSIX_VDISABLE (-1)

// MISSING: intptr_t

int access(const char *path, int mode);
unsigned int alarm(unsigned int seconds);
int chdir(const char *path);
int chown(const char *path, uid_t uid, gid_t gid);
int close(int fd);
ssize_t confstr(int, char *, size_t);
char *crypt(const char *, const char *);
int dup(int fd);
int dup2(int src_fd, int dest_fd);
__attribute__ ((noreturn)) void _exit(int status);
void encrypt(char block[64], int flags);
int execl(const char *, const char *, ...);
int execle(const char *, const char *, ...);
int execlp(const char *, const char *, ...);
int execv(const char *, char *const []);
int execve(const char *path, char *const argv[], char *const envp[]);
int execvp(const char *, char *const[]);
int faccessat(int, const char *, int, int);
int fchdir(int fd);
int fchown(int fd, uid_t uid, gid_t gid);
int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flags);
int fdatasync(int);
int fexecve(int, char *const [], char *const []);
pid_t fork(void);
long fpathconf(int, int);
int fsync(int);
int ftruncate(int, off_t);
char *getcwd(char *, size_t);
gid_t getegid(void);
uid_t geteuid(void);
gid_t getgid(void);
int getgroups(int, gid_t []);
long gethostid(void);
int gethostname(char *buffer, size_t max_length);
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
int isatty(int fd);
int lchown(const char *path, uid_t uid, gid_t gid);
int link(const char *, const char *);
int linkat(int, const char *, int, const char *, int);
int lockf(int, int, off_t);
off_t lseek(int fd, off_t offset, int whence);
int nice(int);
long pathconf(const char *, int);
int pause(void);
int pipe(int [2]);
ssize_t pread(int, void *, size_t, off_t);
ssize_t pwrite(int, const void *, size_t, off_t);
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
unsigned int sleep(unsigned int);
void swab(const void *__restrict, void *__restrict, ssize_t);
int symlink(const char *, const char *);
int symlinkat(const char *, int, const char *);
void sync(void);
unsigned long sysconf(int);
pid_t tcgetpgrp(int);
int tcsetpgrp(int, pid_t);
int truncate(const char *, off_t);
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

// Non-POSIX functions supported by Linux.
typedef __mlibc_uint64 useconds_t;

int getpagesize(void);
char *get_current_dir_name(void);
int usleep(useconds_t);
int chroot(const char *);

// This is a Linux extension
pid_t gettid(void);
int getentropy(void *, size_t);

int pipe2(int *pipefd, int flags);

#ifdef __cplusplus
}
#endif

#if __MLIBC_LINUX_OPTION
#	include <bits/linux/linux_unistd.h>
#endif

#endif // _UNISTD_H


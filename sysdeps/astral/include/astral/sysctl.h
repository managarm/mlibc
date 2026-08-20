#ifndef _ASTRAL_SYSCTL_H
#define _ASTRAL_SYSCTL_H

#include <bits/size_t.h>
#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/pid_t.h>
#include <bits/ansi/timespec.h>

#define SYS_CTL_NAME_MAX 6

#define SYS_CTL_KERN 1
#define SYS_CTL_HW 2
#define SYS_CTL_KERN_PROC 1
#define SYS_CTL_KERN_PROC_ALL 1
#define SYS_CTL_KERN_CONSOLE_LOCK 2
#define SYS_CTL_HW_CPUS_ONLINE 1

#define SYS_CTL_KERN_PROC_INFO_NAME_SIZE 64
#define SYS_CTL_KERN_PROC_INFO_TTY_NAME_SIZE 64

typedef struct {
	pid_t pid;
	uid_t uid;
	uid_t euid;
	uid_t suid;
	gid_t gid;
	gid_t egid;
	gid_t sgid;
	size_t thread_count;
	char name[SYS_CTL_KERN_PROC_INFO_NAME_SIZE];
	pid_t ppid;
	pid_t pgid;
	pid_t sid;
	struct timespec start_timestamp;
	struct timespec total_runtime;
	int state;
	int cpu_time;
	int nice;
	size_t virtual_pages;
	size_t physical_pages;
	char tty_name[SYS_CTL_KERN_PROC_INFO_TTY_NAME_SIZE];
} sysctl_proc_info_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int sysctl(const int *__name, size_t __name_len, void *__oldp, size_t *__old_lenp, void *__newp, size_t __new_len);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ASTRAL_SYSCTL_H */

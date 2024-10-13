
#ifndef _SPAWN_H
#define _SPAWN_H

#include <abi-bits/signal.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <sched.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int __flags;
	pid_t __pgrp;
	sigset_t __def, __mask;
	int __prio, __pol;
	void *__fn;
	char __pad[64 - sizeof(void *)];
} posix_spawnattr_t;

typedef struct {
	int __pad0[2];
	void *__actions;
	int __pad[16];
} posix_spawn_file_actions_t;

/* MISSIG: sigset_t */

struct sched_param;

#define POSIX_SPAWN_RESETIDS 1
#define POSIX_SPAWN_SETPGROUP 2
#define POSIX_SPAWN_SETSIGDEF 4
#define POSIX_SPAWN_SETSIGMASK 8
#define POSIX_SPAWN_SETSCHEDPARAM 16
#define POSIX_SPAWN_SETSCHEDULER 32
#define POSIX_SPAWN_USEVFORK 64
#define POSIX_SPAWN_SETSID 128

#ifndef __MLIBC_ABI_ONLY

int posix_spawn(pid_t *__restrict __pid, const char *__restrict __path,
		const posix_spawn_file_actions_t *__file_actions,
		const posix_spawnattr_t *__restrict __attrs,
		char *const __argv[], char *const __envp[]);

int posix_spawnattr_init(posix_spawnattr_t *__attr);
int posix_spawnattr_destroy(posix_spawnattr_t *__attr);
int posix_spawnattr_setflags(posix_spawnattr_t *__attr, short __flags);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *__restrict __attr,
		const sigset_t *__restrict __sigdefault);
int posix_spawnattr_setschedparam(posix_spawnattr_t *__restrict __attr,
		const struct sched_param *__restrict __schedparam);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *__attr, int __schedpolicy);
int posix_spawnattr_setsigmask(posix_spawnattr_t *__restrict __attr,
		const sigset_t *__restrict __sigmask);
int posix_spawnattr_setpgroup(posix_spawnattr_t *__attr, pid_t __pgroup);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *__file_actions);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *__file_actions);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *__file_actions,
		int __fildes, int __newfildes);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *__file_actions,
		int __fildes);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *__restrict __file_actions,
		int __fildes, const char *__restrict __path, int __oflag, mode_t __mode);
int posix_spawnp(pid_t *__restrict __pid, const char *__restrict __file,
		const posix_spawn_file_actions_t *__file_actions,
		const posix_spawnattr_t *__restrict __attrp,
		char *const __argv[], char *const __envp[]);

/* MISSING: all other functions */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* SPAWN_H */


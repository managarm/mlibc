
#ifndef _SPAWN_H
#define _SPAWN_H

#include <abi-bits/signal.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <sched.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { } posix_spawnattr_t;
typedef struct { } posix_spawn_file_actions_t;

// MISSIG: sigset_t

struct sched_param;

#define POSIX_SPAWN_RESETIDS 1
#define POSIX_SPAWN_SETPGROUP 2
#define POSIX_SPAWN_SETSIGDEF 4
#define POSIX_SPAWN_SETSIGMASK 8
#define POSIX_SPAWN_SETSCHEDPARAM 16
#define POSIX_SPAWN_SETSCHEDULER 32
#define POSIX_SPAWN_USEVFORK 64
#define POSIX_SPAWN_SETSID 128

int posix_spawn(pid_t *__restrict pid, const char *__restrict path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrs,
		char *const argv[], char *const envp[]);

int posix_spawnattr_init(posix_spawnattr_t *attr);
int posix_spawnattr_destroy(posix_spawnattr_t *attr);
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *__restrict attr,
		const sigset_t *__restrict sigdefault);
int posix_spawnattr_setschedparam(posix_spawnattr_t *__restrict attr,
		const struct sched_param *__restrict schedparam);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy);
int posix_spawnattr_setsigmask(posix_spawnattr_t *__restrict attr,
		const sigset_t *__restrict sigmask);
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
		int fildes, int newfildes);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
		int fildes);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *__restrict file_actions,
		int fildes, const char *__restrict path, int oflag, mode_t mode);
int posix_spawnp(pid_t *__restrict pid, const char *__restrict file,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrp,
		char *const argv[], char *const envp[]);

// MISSING: all other functions

#ifdef __cplusplus
}
#endif

#endif // SPAWN_H



#ifndef _SPAWN_H
#define _SPAWN_H

#include <bits/posix/mode_t.h>
#include <bits/posix/pid_t.h>

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

// MISSING: all other functions

#ifdef __cplusplus
}
#endif

#endif // SPAWN_H


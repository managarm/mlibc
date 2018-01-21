
#ifndef _SPAWN_H
#define _SPAWN_H

#include <mlibc/mode_t.h>
#include <mlibc/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { } posix_spawnattr_t;
typedef struct { } posix_spawn_file_actions_t;

// MISSIG: sigset_t

struct sched_param;

// MISSING: POSIX_SPAWN macros

int posix_spawn(pid_t *__restrict pid, const char *__restrict path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrs,
		char *const argv[], char *const envp[]);

// MISSING: all other functions

#ifdef __cplusplus
}
#endif

#endif // SPAWN_H


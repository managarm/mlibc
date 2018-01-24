
#include <spawn.h>

#include <bits/ensure.h>

int posix_spawn(pid_t *__restrict pid, const char *__restrict path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrs,
		char *const argv[], char *const envp[]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


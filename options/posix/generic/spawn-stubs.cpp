
#include <spawn.h>

#include <bits/ensure.h>

int posix_spawn(pid_t *__restrict pid, const char *__restrict path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrs,
		char *const argv[], char *const envp[]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawnattr_init(posix_spawnattr_t *attr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawnattr_setsigdefault(posix_spawnattr_t *__restrict attr,
		const sigset_t *__restrict sigdefault) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
		int fildes, int newfildes) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
		int fildes) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int posix_spawnp(pid_t *__restrict pid, const char *__restrict file,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrp,
		char *const argv[], char *const envp[]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


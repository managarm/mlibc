#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)
#define posix_spawn_file_actions_addchdir posix_spawn_file_actions_addchdir_np
#define posix_spawn_file_actions_addfchdir posix_spawn_file_actions_addfchdir_np
#endif

#ifdef USE_HOST_LIBC
#define TEST_FILE "spawn-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "spawn-cross-libc.tmp"
#else
#define TEST_FILE "spawn.tmp"
#endif

extern char **environ;

void run_cmd(char *cmd) {
	pid_t pid;
	char *argv[] = {"sh", "-c", cmd, NULL};
	int status;
	printf("Run command: %s\n", cmd);
	status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
	if (status == 0) {
		printf("Child pid: %i\n", pid);
		if (waitpid(pid, &status, 0) != -1) {
			printf("Child exited with status %i\n", status);
			printf("Child exit status: %i\n", WEXITSTATUS(status));
			assert(WEXITSTATUS(status) == 0);
		} else {
			perror("waitpid");
			assert(0 == 1);
		}
	} else {
		printf("posix_spawn: %s\n", strerror(status));
		assert(0 == 1);
	}
}

static int child_file_actions(void) {
	int ret = fcntl(3, F_GETFD);
	if (ret != -1 || errno != EBADF) {
		return 10;
	}

	ret = fcntl(4, F_GETFD);
	if (ret < 0) {
		return 11;
	}

	char buf[10];
	ssize_t n = read(4, buf, sizeof(buf));
	if (n != 6 || memcmp(buf, "hello\n", 6) != 0) {
		return 12;
	}

	return 0;
}

static int child_attributes(void) {
	pid_t pid = getpid();

	pid_t sid = getsid(0);
	if (sid != pid) {
		return 20;
	}
	pid_t parent_sid = getsid(getppid());
	if (sid == parent_sid) {
		return 27;
	}

	pid_t pgid = getpgid(0);
	if (pgid != pid) {
		return 21;
	}

	sigset_t sigmask;
	int ret = sigprocmask(SIG_SETMASK, NULL, &sigmask);
	if (ret != 0) {
		return 22;
	}
	if (sigismember(&sigmask, SIGUSR1) != 1) {
		return 23;
	}
	if (sigismember(&sigmask, SIGUSR2) != 0) {
		return 26;
	}

	struct sigaction sa;
	ret = sigaction(SIGUSR2, NULL, &sa);
	if (ret != 0) {
		return 24;
	}
	if (sa.sa_handler != SIG_DFL) {
		return 25;
	}

	if (geteuid() != getuid()) {
		return 28;
	}
	if (getegid() != getgid()) {
		return 29;
	}

	return 0;
}

static int do_spawn(
    pid_t *pid,
    const char *exec_path,
    const posix_spawn_file_actions_t *fa,
    const posix_spawnattr_t *attr,
    const char *action,
    const char *arg
) {
	char *wrapper = getenv("MESON_EXE_WRAPPER");
	char **child_argv = NULL;
	const char *spawn_path = NULL;
	int ret = 0;
	if (wrapper && *wrapper) {
		// Count tokens
		int wrapper_argc = 0;
		char *wrapper_copy = strdup(wrapper);
		char *token = strtok(wrapper_copy, " ");
		while (token) {
			wrapper_argc++;
			token = strtok(NULL, " ");
		}
		free(wrapper_copy);

		child_argv = malloc((wrapper_argc + 4) * sizeof(char *));
		assert(child_argv);

		wrapper_copy = strdup(wrapper);
		token = strtok(wrapper_copy, " ");
		int idx = 0;
		while (token) {
			child_argv[idx++] = strdup(token);
			token = strtok(NULL, " ");
		}
		free(wrapper_copy);

		child_argv[idx++] = (char *)exec_path;
		child_argv[idx++] = (char *)action;
		if (arg) {
			child_argv[idx++] = (char *)arg;
		}
		child_argv[idx] = NULL;

		spawn_path = child_argv[0];

		ret = posix_spawnp(pid, spawn_path, fa, attr, child_argv, environ);

		for (int i = 0; i < wrapper_argc; i++)
			free(child_argv[i]);
		free(child_argv);
	} else {
		child_argv = malloc(4 * sizeof(char *));
		assert(child_argv);
		child_argv[0] = (char *)exec_path;
		child_argv[1] = (char *)action;
		int idx = 2;
		if (arg) {
			child_argv[idx++] = (char *)arg;
		}
		child_argv[idx] = NULL;

		spawn_path = exec_path;

		ret = posix_spawnp(pid, spawn_path, fa, attr, child_argv, environ);

		free(child_argv);
	}
	return ret;
}

static void test_file_actions(const char *exec_path) {
	int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	ssize_t written = write(fd, "hello\n", 6);
	assert(written == 6);
	close(fd);

	posix_spawn_file_actions_t fa;
	int ret = posix_spawn_file_actions_init(&fa);
	assert(ret == 0);

	ret = posix_spawn_file_actions_addopen(&fa, 3, TEST_FILE, O_RDONLY, 0);
	assert(ret == 0);

	ret = posix_spawn_file_actions_adddup2(&fa, 3, 4);
	assert(ret == 0);

	ret = posix_spawn_file_actions_addclose(&fa, 3);
	assert(ret == 0);

	pid_t pid;
	ret = do_spawn(&pid, exec_path, &fa, NULL, "child_file_actions", NULL);
	assert(ret == 0);

	int status = 0;
	pid_t wait_ret = waitpid(pid, &status, 0);
	assert(wait_ret == pid);
	assert(WIFEXITED(status));
	int exit_status = WEXITSTATUS(status);
	assert(exit_status == 0);

	ret = posix_spawn_file_actions_destroy(&fa);
	assert(ret == 0);

	unlink(TEST_FILE);
}

static void dummy_handler(int sig) { (void)sig; }

static void test_attributes(const char *exec_path) {
	posix_spawnattr_t attr;
	int ret = posix_spawnattr_init(&attr);
	assert(ret == 0);

	sigset_t parent_mask, old_parent_mask;
	sigemptyset(&parent_mask);
	sigaddset(&parent_mask, SIGUSR2);
	ret = sigprocmask(SIG_BLOCK, &parent_mask, &old_parent_mask);
	assert(ret == 0);

	struct sigaction new_sa, old_sa;
	new_sa.sa_handler = dummy_handler;
	sigemptyset(&new_sa.sa_mask);
	new_sa.sa_flags = 0;
	ret = sigaction(SIGUSR2, &new_sa, &old_sa);
	assert(ret == 0);

	short flags = POSIX_SPAWN_SETSID | POSIX_SPAWN_SETSIGMASK | POSIX_SPAWN_SETSIGDEF
	              | POSIX_SPAWN_RESETIDS | POSIX_SPAWN_USEVFORK;

	struct sched_param sp;
	ret = sched_getparam(0, &sp);
	assert(ret == 0);
	int policy = sched_getscheduler(0);
	assert(policy >= 0);

	ret = posix_spawnattr_setschedparam(&attr, &sp);
	assert(ret == 0);
	ret = posix_spawnattr_setschedpolicy(&attr, policy);
	assert(ret == 0);

	struct sched_param sp_get;
	ret = posix_spawnattr_getschedparam(&attr, &sp_get);
	assert(ret == 0);
	assert(sp_get.sched_priority == sp.sched_priority);

	int policy_get = -1;
	ret = posix_spawnattr_getschedpolicy(&attr, &policy_get);
	assert(ret == 0);
	assert(policy_get == policy);

	ret = posix_spawnattr_setflags(&attr, flags);
	assert(ret == 0);

	ret = posix_spawnattr_setpgroup(&attr, 0);
	assert(ret == 0);

	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
	ret = posix_spawnattr_setsigmask(&attr, &sigmask);
	assert(ret == 0);

	sigset_t sigdefault;
	sigemptyset(&sigdefault);
	sigaddset(&sigdefault, SIGUSR2);
	ret = posix_spawnattr_setsigdefault(&attr, &sigdefault);
	assert(ret == 0);

	short flags_get = -1;
	ret = posix_spawnattr_getflags(&attr, &flags_get);
	assert(ret == 0);
	assert(flags_get == flags);

	pid_t pgroup_get = -1;
	ret = posix_spawnattr_getpgroup(&attr, &pgroup_get);
	assert(ret == 0);
	assert(pgroup_get == 0);

	sigset_t sigmask_get;
	ret = posix_spawnattr_getsigmask(&attr, &sigmask_get);
	assert(ret == 0);
	assert(sigismember(&sigmask_get, SIGUSR1) == 1);
	assert(sigismember(&sigmask_get, SIGUSR2) == 0);

	sigset_t sigdefault_get;
	ret = posix_spawnattr_getsigdefault(&attr, &sigdefault_get);
	assert(ret == 0);
	assert(sigismember(&sigdefault_get, SIGUSR2) == 1);
	assert(sigismember(&sigdefault_get, SIGUSR1) == 0);

	pid_t pid;
	ret = do_spawn(&pid, exec_path, NULL, &attr, "child_attributes", NULL);
	assert(ret == 0);

	ret = sigaction(SIGUSR2, &old_sa, NULL);
	assert(ret == 0);

	ret = sigprocmask(SIG_SETMASK, &old_parent_mask, NULL);
	assert(ret == 0);

	int status = 0;
	pid_t wait_ret = waitpid(pid, &status, 0);
	assert(wait_ret == pid);
	assert(WIFEXITED(status));
	int exit_status = WEXITSTATUS(status);
	assert(exit_status == 0);

	ret = posix_spawnattr_destroy(&attr);
	assert(ret == 0);
}

static int child_chdir(const char *expected_dir) {
	char cwd[1024];
	if (!getcwd(cwd, sizeof(cwd))) {
		perror("getcwd");
		return 30;
	}
	if (strcmp(cwd, expected_dir) != 0) {
		fprintf(stderr, "chdir failed: expected %s, got %s\n", expected_dir, cwd);
		return 31;
	}
	return 0;
}

static void test_chdir(const char *exec_path) {
#ifdef USE_HOST_LIBC
	const char *test_dir = "spawn-host-libc-dir.tmp";
#elif defined(USE_CROSS_LIBC)
	const char *test_dir = "spawn-cross-libc-dir.tmp";
#else
	const char *test_dir = "spawn-dir.tmp";
#endif

	rmdir(test_dir);
	int ret = mkdir(test_dir, 0777);
	assert(ret == 0);

	char *expected_dir = realpath(test_dir, NULL);
	assert(expected_dir != NULL);

	posix_spawn_file_actions_t fa;
	ret = posix_spawn_file_actions_init(&fa);
	assert(ret == 0);

	ret = posix_spawn_file_actions_addchdir(&fa, test_dir);
	assert(ret == 0);

	pid_t pid;
	ret = do_spawn(&pid, exec_path, &fa, NULL, "child_chdir", expected_dir);
	assert(ret == 0);

	int status = 0;
	pid_t wait_ret = waitpid(pid, &status, 0);
	assert(wait_ret == pid);
	assert(WIFEXITED(status));
	int exit_status = WEXITSTATUS(status);
	assert(exit_status == 0);

	ret = posix_spawn_file_actions_destroy(&fa);
	assert(ret == 0);

	free(expected_dir);
	rmdir(test_dir);
}

static void test_fchdir(const char *exec_path) {
#ifdef USE_HOST_LIBC
	const char *test_dir = "spawn-host-libc-fdir.tmp";
#elif defined(USE_CROSS_LIBC)
	const char *test_dir = "spawn-cross-libc-fdir.tmp";
#else
	const char *test_dir = "spawn-fdir.tmp";
#endif

	rmdir(test_dir);
	int ret = mkdir(test_dir, 0777);
	assert(ret == 0);

	char *expected_dir = realpath(test_dir, NULL);
	assert(expected_dir != NULL);

	int dir_fd = open(test_dir, O_RDONLY | O_DIRECTORY);
	assert(dir_fd >= 0);

	posix_spawn_file_actions_t fa;
	ret = posix_spawn_file_actions_init(&fa);
	assert(ret == 0);

	ret = posix_spawn_file_actions_addfchdir(&fa, dir_fd);
	assert(ret == 0);

	pid_t pid;
	ret = do_spawn(&pid, exec_path, &fa, NULL, "child_chdir", expected_dir);
	assert(ret == 0);

	int status = 0;
	pid_t wait_ret = waitpid(pid, &status, 0);
	assert(wait_ret == pid);
	assert(WIFEXITED(status));
	int exit_status = WEXITSTATUS(status);
	assert(exit_status == 0);

	ret = posix_spawn_file_actions_destroy(&fa);
	assert(ret == 0);

	close(dir_fd);
	free(expected_dir);
	rmdir(test_dir);
}

static int child_setpgroup(void) {
	pid_t pid = getpid();
	pid_t pgid = getpgid(0);
	if (pgid != pid) {
		return 40;
	}
	return 0;
}

static void test_setpgroup(const char *exec_path) {
	posix_spawnattr_t attr;
	int ret = posix_spawnattr_init(&attr);
	assert(ret == 0);

	ret = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
	assert(ret == 0);

	ret = posix_spawnattr_setpgroup(&attr, 0);
	assert(ret == 0);

	short flags_get = -1;
	ret = posix_spawnattr_getflags(&attr, &flags_get);
	assert(ret == 0);
	assert(flags_get == POSIX_SPAWN_SETPGROUP);

	pid_t pgroup_get = -1;
	ret = posix_spawnattr_getpgroup(&attr, &pgroup_get);
	assert(ret == 0);
	assert(pgroup_get == 0);

	pid_t pid;
	ret = do_spawn(&pid, exec_path, NULL, &attr, "child_setpgroup", NULL);
	assert(ret == 0);

	int status = 0;
	pid_t wait_ret = waitpid(pid, &status, 0);
	assert(wait_ret == pid);
	assert(WIFEXITED(status));
	int exit_status = WEXITSTATUS(status);
	assert(exit_status == 0);

	ret = posix_spawnattr_destroy(&attr);
	assert(ret == 0);
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "child_file_actions") == 0) {
			return child_file_actions();
		}
		if (strcmp(argv[1], "child_attributes") == 0) {
			return child_attributes();
		}
		if (strcmp(argv[1], "child_chdir") == 0) {
			assert(argc > 2);
			return child_chdir(argv[2]);
		}
		if (strcmp(argv[1], "child_setpgroup") == 0) {
			return child_setpgroup();
		}
		return 127;
	}

	run_cmd(":");
	test_file_actions(argv[0]);
	test_attributes(argv[0]);
	test_chdir(argv[0]);
	test_fchdir(argv[0]);
	test_setpgroup(argv[0]);
	return 0;
}

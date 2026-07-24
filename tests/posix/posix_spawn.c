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
#include <unistd.h>

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
	char *argv[] = {(char *)exec_path, "child_file_actions", NULL};
	ret = posix_spawn(&pid, exec_path, &fa, NULL, argv, environ);
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

	pid_t pid;
	char *argv[] = {(char *)exec_path, "child_attributes", NULL};
	ret = posix_spawn(&pid, exec_path, NULL, &attr, argv, environ);
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

int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "child_file_actions") == 0) {
			return child_file_actions();
		}
		if (strcmp(argv[1], "child_attributes") == 0) {
			return child_attributes();
		}
		return 127;
	}

	run_cmd(":");
	test_file_actions(argv[0]);
	test_attributes(argv[0]);
	return 0;
}

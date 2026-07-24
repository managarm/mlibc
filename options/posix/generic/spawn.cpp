#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <frg/allocation.hpp>
#include <limits.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/spawn-types.hpp>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Musl places this in a seperate header called fdop.h
 * This header isn't present in glibc, or on my host, so I
 * include it's contents here
 */

#define FDOP_CLOSE 1
#define FDOP_DUP2 2
#define FDOP_OPEN 3
#define FDOP_CHDIR 4
#define FDOP_FCHDIR 5

namespace {
/*
 * This posix_spawn implementation is taken from musl
 */

static unsigned long handler_set[NSIG / (8 * sizeof(long))];

void __get_handler_set(sigset_t *set) {
	memcpy(set, handler_set, sizeof handler_set);
}

struct args {
	int p[2];
	sigset_t oldmask;
	const char *path;
	const posix_spawn_file_actions_t *fa;
	const posix_spawnattr_t *__restrict attr;
	char *const *argv, *const *envp;
	int (*exec_fn)(const char *path, char *const argv[], char *const env[]);
};

int child(void *args_vp) {
	int i, ret;
	struct sigaction sa = {};
	struct args *args = (struct args *)args_vp;
	int p = args->p[1];
	const __mlibc_spawnattr *__restrict attr = __mlibc_spawnattr::from(args->attr);
	const __mlibc_spawn_file_actions *fa = __mlibc_spawn_file_actions::from(args->fa);

	sigset_t hset;

	close(args->p[0]);

	int flags = attr ? attr->__flags : 0;

	/* All signal dispositions must be either SIG_DFL or SIG_IGN
	 * before signals are unblocked. Otherwise a signal handler
	 * from the parent might get run in the child while sharing
	 * memory, with unpredictable and dangerous results. To
	 * reduce overhead, sigaction has tracked for us which signals
	 * potentially have a signal handler. */
	__get_handler_set(&hset);
	for(i = 1; i < NSIG; i++) {
		if((flags & POSIX_SPAWN_SETSIGDEF) && sigismember(&attr->__def, i)) {
			sa.sa_handler = SIG_DFL;
		} else if(sigismember(&hset, i)) {
			if (i - 32 < 3) {
				sa.sa_handler = SIG_IGN;
			} else {;
				sigaction(i, nullptr, &sa);
				if(sa.sa_handler == SIG_IGN)
					continue;
				sa.sa_handler = SIG_DFL;
			}
		} else {
			continue;
		}
		sigaction(i, &sa, nullptr);
	}

	if(flags & POSIX_SPAWN_SETSID) {
		if((ret = setsid()) < 0)
			goto fail;
	}

	if(flags & POSIX_SPAWN_SETPGROUP) {
		if(setpgid(0, attr->__pgrp) == -1) {
			ret = -errno;
			goto fail;
		}
	}

	if (flags & POSIX_SPAWN_SETSCHEDULER) {
		if (sched_setscheduler(0, attr->__schedpolicy, reinterpret_cast<const sched_param *>(&attr->__schedparam)) == -1) {
			if (errno != ENOSYS) {
				ret = -errno;
				goto fail;
			}
		}
	} else if (flags & POSIX_SPAWN_SETSCHEDPARAM) {
		if (sched_setparam(0, reinterpret_cast<const sched_param *>(&attr->__schedparam)) == -1) {
			if (errno != ENOSYS) {
				ret = -errno;
				goto fail;
			}
		}
	}

	if(flags & POSIX_SPAWN_RESETIDS) {
		if((ret = setgid(getgid())) || (ret = setuid(getuid())) )
			goto fail;
	}

	if(fa) {
		for (auto &op : fa->ops) {
			/* It's possible that a file operation would clobber
			 * the pipe fd used for synchronizing with the
			 * parent. To avoid that, we dup the pipe onto
			 * an unoccupied fd. */
			if(op.fd == p) {
				ret = dup(p);
				if(ret < 0)
					goto fail;
				close(p);
				p = ret;
			}
			switch(op.cmd) {
			case FDOP_CLOSE:
				close(op.fd);
				break;
			case FDOP_DUP2: {
				int fd = op.srcfd;
				if(fd == p) {
					ret = -EBADF;
					goto fail;
				}
				if(fd != op.fd) {
					if((ret = dup2(fd, op.fd)) < 0)
						goto fail;
				} else {
					ret = fcntl(fd, F_GETFD);
					ret = fcntl(fd, F_SETFD, ret & ~FD_CLOEXEC);
					if(ret < 0)
						goto fail;
				}
				break;
			}
			case FDOP_OPEN: {
				int fd = open(op.path.data(), op.oflag, op.mode);
				if((ret = fd) < 0)
					goto fail;
				if(fd != op.fd) {
					if((ret = dup2(fd, op.fd)) < 0)
						goto fail;
					close(fd);
				}
				break;
			}
			case FDOP_CHDIR:
				ret = chdir(op.path.data());
				if(ret < 0)
					goto fail;
				break;
			case FDOP_FCHDIR:
				ret = fchdir(op.fd);
				if(ret < 0)
					goto fail;
				break;
			}
		}
	}

	/* Close-on-exec flag may have been lost if we moved the pipe
	 * to a different fd. */
	fcntl(p, F_SETFD, FD_CLOEXEC);

	pthread_sigmask(SIG_SETMASK, (flags & POSIX_SPAWN_SETSIGMASK)
		? &attr->__mask : &args->oldmask, nullptr);

	args->exec_fn(args->path, args->argv, args->envp);
	ret = -errno;

fail:
	/* Since sizeof errno < PIPE_BUF, the write is atomic. */
	ret = -ret;
	if(ret)
		while(write(p, &ret, sizeof ret) < 0);
	_exit(127);
}

int posix_spawn_impl(pid_t *__restrict res, const char *__restrict path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrs,
		char *const argv[], char *const envp[],
		int exec_fn(const char *path, char *const argv[], char *const env[])) {
	pid_t pid;
	int ec = 0, cs;
	struct args args;
	sigset_t full_sigset;
	sigfillset(&full_sigset);

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	args.path = path;
	args.fa = file_actions;
	args.attr = attrs ? attrs : nullptr;
	args.argv = argv;
	args.envp = envp;
	args.exec_fn = exec_fn;
	pthread_sigmask(SIG_BLOCK, &full_sigset, &args.oldmask);

	/* The lock guards both against seeing a SIGABRT disposition change
	 * by abort and against leaking the pipe fd to fork-without-exec. */
	//LOCK(__abort_lock);

	if(pipe2(args.p, O_CLOEXEC)) {
		//UNLOCK(__abort_lock);
		ec = errno;
		goto fail;
	}

	/* Mlibc change: We use fork + execve, as clone is not implemented.
	 * This yields the same result in the end. */
	//pid = clone(child, stack + sizeof stack, CLONE_VM | CLONE_VFORK | SIGCHLD, &args);
	pid = fork();
	if(!pid) {
		child(&args);
	}
	close(args.p[1]);
	//UNLOCK(__abort_lock);

	if(pid > 0) {
		if(read(args.p[0], &ec, sizeof ec) != sizeof ec)
			ec = 0;
		else
			waitpid(pid, nullptr, 0);
	} else {
		ec = -pid;
	}

	close(args.p[0]);

	if(!ec && res)
		*res = pid;

fail:
	pthread_sigmask(SIG_SETMASK, &args.oldmask, nullptr);
	pthread_setcancelstate(cs, nullptr);

	return ec;
}

} // namespace

int posix_spawn(pid_t *__restrict res, const char *__restrict path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrs,
		char *const argv[], char *const envp[]) {
	return posix_spawn_impl(res, path, file_actions, attrs, argv, envp, execve);
}

int posix_spawnattr_init(posix_spawnattr_t *attr) {
	new (attr) posix_spawnattr_t(frg::construct<__mlibc_spawnattr>(getAllocator()));
	return 0;
}

int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
	if (attr && attr->__heap_ptr) {
		frg::destruct<__mlibc_spawnattr>(getAllocator(), __mlibc_spawnattr::from(attr));
		attr->__heap_ptr = nullptr;
	}
	return 0;
}

int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
	const unsigned all_flags =
			POSIX_SPAWN_RESETIDS |
			POSIX_SPAWN_SETPGROUP |
			POSIX_SPAWN_SETSIGDEF |
			POSIX_SPAWN_SETSIGMASK |
			POSIX_SPAWN_SETSCHEDPARAM |
			POSIX_SPAWN_SETSCHEDULER |
			POSIX_SPAWN_USEVFORK |
			POSIX_SPAWN_SETSID;
	if(flags & ~all_flags)
		return EINVAL;
	auto a = __mlibc_spawnattr::from(attr);
	a->__flags = flags;
	return 0;
}

int posix_spawnattr_setsigdefault(posix_spawnattr_t *__restrict attr,
		const sigset_t *__restrict sigdefault) {
	auto a = __mlibc_spawnattr::from(attr);
	a->__def = *sigdefault;
	return 0;
}

int posix_spawnattr_getschedparam(
    const posix_spawnattr_t *__restrict attr, struct sched_param *__restrict schedparam
) {
	auto a = __mlibc_spawnattr::from(attr);
	memcpy(schedparam, &a->__schedparam, sizeof(*schedparam));
	return 0;
}

int posix_spawnattr_setschedparam(
    posix_spawnattr_t *__restrict attr, const struct sched_param *__restrict sp
) {
	auto a = __mlibc_spawnattr::from(attr);
	memcpy(&a->__schedparam, sp, sizeof(a->__schedparam));
	return 0;
}

int posix_spawnattr_getschedpolicy(
    const posix_spawnattr_t *__restrict attr, int *__restrict schedpolicy
) {
	auto a = __mlibc_spawnattr::from(attr);
	*schedpolicy = a->__schedpolicy;
	return 0;
}

int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int sp) {
	auto a = __mlibc_spawnattr::from(attr);
	a->__schedpolicy = sp;
	return 0;
}

int posix_spawnattr_setsigmask(posix_spawnattr_t *__restrict attr,
		const sigset_t *__restrict sigmask) {
	auto a = __mlibc_spawnattr::from(attr);
	a->__mask = *sigmask;
	return 0;
}

int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup) {
	auto a = __mlibc_spawnattr::from(attr);
	a->__pgrp = pgroup;
	return 0;
}

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
	new (file_actions) posix_spawn_file_actions_t(frg::construct<__mlibc_spawn_file_actions>(getAllocator()));
	return 0;
}

int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
	if (file_actions && file_actions->__heap_ptr) {
		frg::destruct<__mlibc_spawn_file_actions>(
		    getAllocator(), __mlibc_spawn_file_actions::from(file_actions)
		);
		file_actions->__heap_ptr = nullptr;
	}

	return 0;
}

int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
		int fildes, int newfildes) {
	auto fa = __mlibc_spawn_file_actions::from(file_actions);
	fa->ops.emplace_back(FDOP_DUP2, newfildes, fildes);

	return 0;
}

int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
		int fildes) {
	auto fa = __mlibc_spawn_file_actions::from(file_actions);
	fa->ops.emplace_back(FDOP_CLOSE, fildes);

	return 0;
}

int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *__restrict file_actions,
		int fildes, const char *__restrict path, int oflag, mode_t mode) {
	auto fa = __mlibc_spawn_file_actions::from(file_actions);
	fa->ops.emplace_back(
	    FDOP_OPEN, fildes, -1, oflag, mode, frg::string<MemoryAllocator>{getAllocator(), path}
	);

	return 0;
}

int posix_spawnp(pid_t *__restrict pid, const char *__restrict file,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *__restrict attrp,
		char *const argv[], char *const envp[]) {
	return posix_spawn_impl(pid, file, file_actions, attrp, argv, envp, execvpe);
}

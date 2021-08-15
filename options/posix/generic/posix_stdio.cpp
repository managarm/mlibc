
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/file-io.hpp>
#include <mlibc/posix-file-io.hpp>
#include <mlibc/posix-sysdeps.hpp>

struct popen_file : mlibc::fd_file {
	popen_file(int fd, void (*do_dispose)(abstract_file *) = nullptr)
		: fd_file(fd, do_dispose) {}

	pid_t get_popen_pid() {
		return _popen_pid;
	}

	void set_popen_pid(pid_t new_pid) {
		_popen_pid = new_pid;
	}

private:
	// Underlying PID in case of popen()
	pid_t _popen_pid;
};

FILE *fmemopen(void *__restrict, size_t, const char *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pclose(FILE *stream) {
	if (!mlibc::sys_waitpid) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	auto file = static_cast<popen_file *>(stream);

	int status;
	pid_t pid = file->get_popen_pid();

	fclose(file);

	if (mlibc::sys_waitpid(pid, &status, 0, &pid) != 0) {
	    errno = ECHILD;
	    return -1;
	}

	return status;
}

FILE *popen(const char *command, const char *typestr) {
	bool is_write;
	pid_t child;
	FILE *ret = nullptr;

	if (!mlibc::sys_fork || !mlibc::sys_dup2 || !mlibc::sys_execve
			|| !mlibc::sys_sigprocmask || !mlibc::sys_sigaction || !mlibc::sys_pipe) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return nullptr;
	}

	if (typestr == NULL) {
		errno = EINVAL;
		return nullptr;
	}

	if (strstr(typestr, "w") != NULL) {
		is_write = true;
	} else if (strstr(typestr, "r") != NULL) {
		is_write = false;
	} else {
		errno = EINVAL;
		return nullptr;
	}

	bool cloexec = false;
	if (strstr(typestr, "e") != NULL) {
		// Set FD_CLOEXEC on the new file descriptor
		cloexec = true;
	}

	int fds[2];
	if (int e = mlibc::sys_pipe(fds, 0)) {
		errno = e;
		return nullptr;
	}

	struct sigaction new_sa, old_int, old_quit;
	sigset_t new_mask, old_mask;

	new_sa.sa_handler = SIG_IGN;
	new_sa.sa_flags = 0;
	sigemptyset(&new_sa.sa_mask);
	mlibc::sys_sigaction(SIGINT, &new_sa, &old_int);
	mlibc::sys_sigaction(SIGQUIT, &new_sa, &old_quit);

	sigemptyset(&new_mask);
	sigaddset(&new_mask, SIGCHLD);
	mlibc::sys_sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

	int parent_end = is_write ? 1 : 0;
	int child_end = is_write ? 0 : 1;

	if (int e = mlibc::sys_fork(&child)) {
		errno = e;
		mlibc::sys_close(fds[0]);
		mlibc::sys_close(fds[1]);
	} else if (!child) {
		// For the child
		mlibc::sys_sigaction(SIGINT, &old_int, nullptr);
		mlibc::sys_sigaction(SIGQUIT, &old_quit, nullptr);
		mlibc::sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);

		mlibc::sys_close(fds[parent_end]);

		if (int e = mlibc::sys_dup2(fds[child_end], 0, is_write ? 0 : 1)) {
			__ensure(!"sys_dup2() failed in popen()");
		}
		mlibc::sys_close(fds[child_end]);

		const char *args[] = {
			"sh", "-c", command, nullptr
		};

		mlibc::sys_execve("/bin/sh", const_cast<char **>(args), environ);
		_Exit(127);
	} else {
		// For the parent
		mlibc::sys_close(fds[child_end]);

		ret = frg::construct<popen_file>(
			getAllocator(),
			fds[parent_end],
			[] (mlibc::abstract_file *abstract) {
				frg::destruct(getAllocator(), abstract);
			}
		);
		__ensure(ret);

		auto file = static_cast<popen_file *>(ret);

		file->set_popen_pid(child);

		if (cloexec == true) {
			fcntl(file->fd(), F_SETFD, O_CLOEXEC);
		}
	}

	mlibc::sys_sigaction(SIGINT, &old_int, nullptr);
	mlibc::sys_sigaction(SIGQUIT, &old_quit, nullptr);
	mlibc::sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);

	return ret;
}

FILE *open_memstream(char **buf, size_t *sizeloc) {
	return frg::construct<mlibc::mem_file>(getAllocator(), buf, sizeloc,
			[] (mlibc::abstract_file *abstract) { frg::destruct(getAllocator(), abstract); });
}

int fseeko(FILE *file_base, off_t offset, int whence) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(int e = file->seek(offset, whence); e) {
		errno = e;
		return -1;
	}
	return 0;
}

off_t ftello(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	off_t current_offset;
	if(int e = file->tell(&current_offset); e) {
		errno = e;
		return -1;
	}
	return current_offset;
}

int dprintf(int fd, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int result = vdprintf(fd, format, args);
	va_end(args);
	return result;
}

int vdprintf(int fd, const char *format, __gnuc_va_list args) {
	mlibc::fd_file file{fd};
	int ret = vfprintf(&file, format, args);
	file.flush();
	return ret;
}


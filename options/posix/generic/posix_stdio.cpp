
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/file-io.hpp>

FILE *fmemopen(void *__restrict, size_t, const char *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pclose(FILE *fd) {
	close(fd->__popen_pipe);

	int status;

	if(waitpid(fd->__popen_pid, &status, 0) < 0) {
		return -1;
	}

	return status;
}

FILE *popen(const char* cmd, const char *mode) {
	mlibc::infoLogger() << "\e[31mmlibc: popen()" << "\e[39m" << frg::endlog;

	int pipe_fd[2];
	pid_t child_pid;
	FILE *f;

	if(*mode != 'r' && *mode != 'w') {
		errno = EINVAL;
		return NULL;
	}

	bool writing = (*mode == 'w') ? true : false;
	int redirect = writing ? 0 /* stdin */ : 1 /* stdout */;
	int used_end, unused_end;

	if(pipe(pipe_fd)) {
		return NULL;
	}

	if(*mode == 'r') {
		f = fdopen(pipe_fd[0], mode);
	} else {
		f = fdopen(pipe_fd[1], mode);
	}

	if(!f) {
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return NULL;
	}

	if((child_pid = fork()) < 0) {
		goto fail;
	}

	if(child_pid) {
		used_end = writing ? 1 /* writing */ : 0 /* reading */;
		unused_end = writing ? 0 /* reading */ : 1 /* writing */;

		FILE *fd = frg::construct<mlibc::fd_file>(getAllocator(), pipe_fd[used_end], [] (mlibc::abstract_file *abstract) { frg::destruct(getAllocator(), abstract); });
		fd->__popen_pid = child_pid;
		close(pipe_fd[unused_end]);

		return fd;
	}

	used_end = writing ? 0 /* reading */ : 1 /* writing */;
	unused_end = writing ? 1 /* writing */ : 0 /* reading */;

	if(dup2(pipe_fd[used_end], redirect) || close(pipe_fd[used_end]) || close(pipe_fd[unused_end])) {
		_exit(127);
	}

	execl("/bin/sh", "sh", "-c", cmd, NULL);
	_exit(127);

fail:
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	return NULL;
 }

FILE *open_memstream(char **, size_t *) {
	mlibc::infoLogger() << "\e[31mmlibc: open_memstream() always fails"
			<< "\e[39m" << frg::endlog;
	errno = ENOMEM;
	return nullptr;
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


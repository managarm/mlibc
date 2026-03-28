#include <array>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <optional>
#include <pthread.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <utmpx.h>

#include <bits/ensure.h>
#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/arch-defs.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/getopt.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/utmp.hpp>

namespace {

constexpr bool logExecvpeTries = false;

} // namespace

unsigned int alarm(unsigned int seconds) {
	struct itimerval it = {}, old = {};
	it.it_value.tv_sec = seconds;
	setitimer(ITIMER_REAL, &it, &old);
	return old.it_value.tv_sec + !! old.it_value.tv_usec;
}

int chdir(const char *path) {
	if(int e = mlibc::sysdep_or_enosys<Chdir>(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchdir(int fd) {
	if(int e = mlibc::sysdep_or_enosys<Fchdir>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int chown(const char *path, uid_t uid, gid_t gid) {
	if(int e = mlibc::sysdep_or_enosys<Fchownat>(AT_FDCWD, path, uid, gid, 0); e) {
		errno = e;
		return -1;
	}
	return 0;
}

size_t confstr(int name, char *buf, size_t len) {
	const char *str = "";
	if (name == _CS_PATH) {
		str = "/bin:/usr/bin";
	} else if(name == _CS_GNU_LIBPTHREAD_VERSION) {
		// We are not glibc, so we can return 0 here.
		return 0;
	} else if(name == _CS_GNU_LIBC_VERSION) {
		// We are not glibc, so we can return 0 here.
		return 0;
	} else {
		mlibc::infoLogger() << "\e[31mmlibc: confstr() request " << name << " is unimplemented\e[39m"
				<< frg::endlog;
		__ensure(!"Not implemented");
	}

	return snprintf(buf, len, "%s", str) + 1;
}

void _exit(int status) {
	mlibc::sysdep<Exit>(status);
}

int execl(const char *path, const char *arg0, ...) {
	// TODO: It's a stupid idea to limit the number of arguments here.
	char *argv[16];
	argv[0] = const_cast<char *>(arg0);

	va_list args;
	int n = 1;
	va_start(args, arg0);
	while(true) {
		__ensure(n < 15);
		auto argn = va_arg(args, const char *);
		argv[n++] = const_cast<char *>(argn);
		if(!argn)
			break;
	}
	va_end(args);
	argv[n] = nullptr;

	return execve(path, argv, environ);
}

// This function is taken from musl.
int execle(const char *path, const char *arg0, ...) {
	int argc;
	va_list ap;
	va_start(ap, arg0);
	for(argc = 1; va_arg(ap, const char *); argc++);
	va_end(ap);

	int i;
	char *argv[argc + 1];
	char **envp;
	va_start(ap, arg0);
	argv[0] = (char *)arg0;
	for(i = 1; i <= argc; i++)
		argv[i] = va_arg(ap, char *);
	envp = va_arg(ap, char **);
	va_end(ap);
	return execve(path, argv, envp);
}

// This function is taken from musl
int execlp(const char *file, const char *argv0, ...) {
	int argc;
	va_list ap;
	va_start(ap, argv0);
	for(argc = 1; va_arg(ap, const char *); argc++);
	va_end(ap);
	{
		int i;
		char *argv[argc + 1];
		va_start(ap, argv0);
		argv[0] = (char *)argv0;
		for(i = 1; i < argc; i++)
			argv[i] = va_arg(ap, char *);
		argv[i] = nullptr;
		va_end(ap);
		return execvp(file, argv);
	}
}

int execv(const char *path, char *const argv[]) {
	return execve(path, argv, environ);
}

int execvp(const char *file, char *const argv[]) {
	return execvpe(file, argv, environ);
}

int execvpe(const char *file, char *const argv[], char *const envp[]) {
	char *null_list[] = {
		nullptr
	};

	if(!argv)
		argv = null_list;
	if(!envp)
		envp = null_list;

	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Execve>, -1);

	if(strchr(file, '/')) {
		int e = mlibc::sysdep_or_panic<Execve>(file, argv, envp);
		__ensure(e && "sys_execve() is supposed to never return with success");
		errno = e;
		return -1;
	}

	frg::string_view dirs;
	if(const char *pv = getenv("PATH"); pv) {
		dirs = pv;
	}else{
		dirs = "/bin:/usr/bin";
	}

	size_t p = 0;
	int res = ENOENT;
	while(p < dirs.size()) {
		size_t s; // Offset of next colon or end of string.
		if(size_t cs = dirs.find_first(':', p); cs != size_t(-1)) {
			s = cs;
		}else{
			s = dirs.size();
		}

		frg::string<MemoryAllocator> path{getAllocator()};
		path += dirs.sub_string(p, s - p);
		path += "/";
		path += file;

		if(logExecvpeTries)
			mlibc::infoLogger() << "mlibc: execvpe() tries '" << path.data() << "'" << frg::endlog;

		int e = mlibc::sysdep_or_panic<Execve>(path.data(), argv, envp);
		__ensure(e && "sys_execve() is supposed to never return with success");
		switch(e) {
		case ENOENT:
		case ENOTDIR:
			break;
		case EACCES:
			res = EACCES;
			break;
		default:
			errno = e;
			return -1;
		}

		p = s + 1;
	}

	errno = res;
	return -1;
}

int faccessat(int dirfd, const char *pathname, int mode, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Faccessat>(dirfd, pathname, mode, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchown(int fd, uid_t uid, gid_t gid) {
	if(int e = mlibc::sysdep_or_enosys<Fchownat>(fd, "", uid, gid, AT_EMPTY_PATH); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Fchownat>(fd, path, uid, gid, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fdatasync(int fd) {
	if(int e = mlibc::sysdep_or_enosys<Fdatasync>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fexecve(int fd, char *const argv[], char *const envp[]) {
	if (int e = mlibc::sysdep_or_enosys<Fexecve>(fd, argv, envp); e) {
		errno = e;
		return -1;
	}
	return 0;
}

long fpathconf(int, int name) {
	switch (name) {
	case _PC_NAME_MAX:
		return NAME_MAX;
	case _PC_FILESIZEBITS:
		return FILESIZEBITS;
	default:
		mlibc::infoLogger() << "missing fpathconf() entry " << name << frg::endlog;
		errno = EINVAL;
		return -1;
	}
}

int fsync(int fd) {
	if(auto e = mlibc::sysdep_or_enosys<Fsync>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int ftruncate(int fd, off_t size) {
	if(int e = mlibc::sysdep_or_enosys<Ftruncate>(fd, size); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("ftruncate")]] int ftruncate64(int fd, off64_t size);
#endif /* !__MLIBC_LINUX_OPTION */

char *getcwd(char *buffer, size_t size) {
	if (buffer) {
		if (size == 0) {
			errno = EINVAL;
			return nullptr;
		}
	} else if (!buffer) {
		if (size == 0)
			size = PATH_MAX;

		buffer = (char *)malloc(size);
	}

	if constexpr (mlibc::IsImplemented<GetCwd>) {
		if(int e = mlibc::sysdep_or_panic<GetCwd>(buffer, size); e) {
			errno = e;
			return nullptr;
		}
		return buffer;
	}

	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Stat>, nullptr);
	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Openat>, nullptr);
	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Dup>, nullptr);

	struct stat root_stat;
	if (int e = mlibc::sysdep_or_panic<Stat>(mlibc::fsfd_target::fd_path, AT_FDCWD,
	                            "/", AT_SYMLINK_NOFOLLOW,
	                            &root_stat); e) {
		errno = e;
		return nullptr;
	}

	struct stat cur_dir_stat;
	if (int e = mlibc::sysdep_or_panic<Stat>(mlibc::fsfd_target::fd_path, AT_FDCWD,
	                            ".", AT_SYMLINK_NOFOLLOW,
	                            &cur_dir_stat); e) {
		errno = e;
		return nullptr;
	}

	if (cur_dir_stat.st_ino == root_stat.st_ino
	 && cur_dir_stat.st_dev == root_stat.st_dev) {
		if (size < 2) {
			errno = ERANGE;
			return nullptr;
		}
		strcpy(buffer, "/");
		return buffer;
	}

	size_t bufptr = size - 1;
	buffer[bufptr] = 0;

	int par_dir = AT_FDCWD;
	bool last_run = false;

	for (;;) {
		int old_par_dir = par_dir;
		if (int e = mlibc::sysdep_or_panic<Openat>(old_par_dir, "..", O_RDONLY, 0, &par_dir); e) {
			errno = e;
			return nullptr;
		}
		if (old_par_dir != AT_FDCWD) {
			mlibc::sysdep<Close>(old_par_dir);
		}

		struct stat par_dir_stat;
		if (int e = mlibc::sysdep_or_panic<Stat>(mlibc::fsfd_target::fd, par_dir, nullptr,
		                            0, &par_dir_stat); e) {
			mlibc::sysdep<Close>(par_dir);
			errno = e;
			return nullptr;
		}

		int par_dir_copy;
		if (int e = mlibc::sysdep_or_panic<Dup>(par_dir, 0, &par_dir_copy); e) {
			mlibc::sysdep<Close>(par_dir);
			errno = e;
			return nullptr;
		}

		DIR *par_dir_dir = fdopendir(par_dir_copy);
		if (par_dir_dir == nullptr) {
			mlibc::sysdep<Close>(par_dir_copy);
			mlibc::sysdep<Close>(par_dir);
			return nullptr;
		}

		if (par_dir_stat.st_ino == root_stat.st_ino
		 && par_dir_stat.st_dev == root_stat.st_dev) {
			last_run = true;
		}

		for (;;) {
			struct dirent *cur_ent = readdir(par_dir_dir);
			if (cur_ent == nullptr) {
				closedir(par_dir_dir);
				mlibc::sysdep<Close>(par_dir);
				return nullptr;
			}

			if (strcmp(cur_ent->d_name, ".") == 0 || strcmp(cur_ent->d_name, "..") == 0) {
				continue;
			}

			struct stat cur_ent_stat;
			if (int e = mlibc::sysdep_or_panic<Stat>(mlibc::fsfd_target::fd_path, par_dir,
			                            cur_ent->d_name, AT_SYMLINK_NOFOLLOW,
			                            &cur_ent_stat)) {
				closedir(par_dir_dir);
				mlibc::sysdep<Close>(par_dir);
				errno = e;
				return nullptr;
			}

			if (cur_ent_stat.st_ino == cur_dir_stat.st_ino
			 && cur_ent_stat.st_dev == cur_dir_stat.st_dev) {
				size_t len = strlen(cur_ent->d_name);
				if (len + 1 > bufptr + 1) {
					closedir(par_dir_dir);
					mlibc::sysdep<Close>(par_dir);
					errno = ERANGE;
					return nullptr;
				}
				bufptr -= len;
				memcpy(&buffer[bufptr], cur_ent->d_name, len);
				bufptr--;
				buffer[bufptr] = '/';
				break;
			}
		}

		closedir(par_dir_dir);

		cur_dir_stat = par_dir_stat;

		if (last_run) {
			break;
		}
	}

	mlibc::sysdep<Close>(par_dir);

	memmove(buffer, &buffer[bufptr], strlen(&buffer[bufptr]) + 1);
	return buffer;
}

int getgroups(int size, gid_t list[]) {
	int ret;
	if(int e = mlibc::sysdep_or_enosys<GetGroups>(size, list, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

long gethostid(void) {
	mlibc::infoLogger() << "mlibc: " << __FUNCTION__ << " not implemented!" << frg::endlog;
	return errno = ENOSYS, -1;
}

int gethostname(char *buffer, size_t bufsize) {
	if(auto e = mlibc::sysdep_or_enosys<GetHostname>(buffer, bufsize); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sethostname(const char *buffer, size_t bufsize) {
	if(auto e = mlibc::sysdep_or_enosys<SetHostname>(buffer, bufsize); e) {
		errno = e;
		return -1;
	}
	return 0;
}

namespace {

char getloginBuf[LOGIN_NAME_MAX];

} // namespace

char *getlogin(void) {
	if (int e = getlogin_r(getloginBuf, sizeof(getloginBuf)); e) {
		errno = e;
		return nullptr;
	}
	return getloginBuf;
}

int getlogin_r(char *name, size_t name_len) {
	if constexpr (mlibc::IsImplemented<GetLoginR>) {
		int e = mlibc::sysdep_or_enosys<GetLoginR>(name, name_len);
		if (e == 0) {
			return 0;
		} else if (e && e != EINVAL) {
			return e;
		}
	}

	char ttypath[2 * NAME_MAX + 3];
	if (int e = ttyname_r(0, ttypath, sizeof(ttypath)); e)
		return e;

	struct utmpx line;
	strlcpy(line.ut_line, ttypath + 5, sizeof(line.ut_line));

	struct utmpx utx;
	if (!mlibc::getutxline_r(&line, &utx)) {
		if (errno == ESRCH)
			return ENOENT;
		return errno;
	}

	size_t needed = strnlen(utx.ut_user, __UT_NAMESIZE) + 1;
	if (needed > name_len)
		return ERANGE;

	strlcpy(name, utx.ut_user, needed);
	return 0;
}

int getopt(int argc, char *const argv[], const char *optstring) {
	return mlibc::getopt_common(argc, argv, optstring, nullptr, nullptr, mlibc::GetoptMode::Short);
}

pid_t getpgid(pid_t pid) {
	pid_t pgid;

	if(int e = mlibc::sysdep_or_enosys<GetPgid>(pid, &pgid); e) {
		errno = e;
		return -1;
	}
	return pgid;
}

pid_t getpgrp(void) {
	return getpgid(0);
}

pid_t getsid(pid_t pid) {
	pid_t sid;
	if(int e = mlibc::sysdep_or_enosys<GetSid>(pid, &sid); e) {
		errno = e;
		return -1;
	}
	return sid;
}

int lchown(const char *path, uid_t uid, gid_t gid) {
	if(int e = mlibc::sysdep_or_enosys<Fchownat>(AT_FDCWD, path, uid, gid, AT_SYMLINK_NOFOLLOW); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int link(const char *old_path, const char *new_path) {
	if(int e = mlibc::sysdep_or_enosys<Link>(old_path, new_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Linkat>(olddirfd, old_path, newdirfd, new_path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

// Code take from musl
int lockf(int fd, int op, off_t size) {
	struct flock l = {
		.l_type = F_WRLCK,
		.l_whence = SEEK_CUR,
		.l_start = 0,
		.l_len = size,
		.l_pid = 0,
	};

	switch(op) {
		case F_TEST:
			l.l_type = F_RDLCK;
			if(fcntl(fd, F_GETLK, &l) < 0)
				return -1;
			if(l.l_type == F_UNLCK || l.l_pid == getpid())
				return 0;
			errno = EACCES;
			return -1;
		case F_ULOCK:
			l.l_type = F_UNLCK;
			[[fallthrough]];
		case F_TLOCK:
			return fcntl(fd, F_SETLK, &l);
		case F_LOCK:
			return fcntl(fd, F_SETLKW, &l);
	}

	errno = EINVAL;
	return -1;
}

int nice(int nice) {
	int new_nice;
	if(int e = mlibc::sysdep_or_enosys<Nice>(nice, &new_nice); e) {
		errno = e;
		return -1;
	}

	return new_nice;
}

long pathconf(const char *, int name) {
	switch (name) {
	case _PC_NAME_MAX:
		return NAME_MAX;
	case _PC_FILESIZEBITS:
		return FILESIZEBITS;
	default:
		mlibc::infoLogger() << "missing pathconf() entry " << name << frg::endlog;
		errno = EINVAL;
		return -1;
	}
}

int pause(void) {
	if(int e = mlibc::sysdep_or_enosys<Pause>(); e) {
		errno = e;
		return -1;
	}
	__ensure(!"There is no successful completion return value for pause");
	__builtin_unreachable();
}

int pipe(int *fds) {
	if(int e = mlibc::sysdep_or_enosys<Pipe>(fds, 0); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int pipe2(int *fds, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Pipe>(fds, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t pread(int fd, void *buf, size_t n, off_t off) {
	ssize_t num_read;

	if(int e = mlibc::sysdep_or_enosys<Pread>(fd, buf, n, off, &num_read); e) {
		errno = e;
		return -1;
	}
	return num_read;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("pread")]] ssize_t pread64(int fd, void *buf, size_t n, off_t off);
#endif /* !__MLIBC_LINUX_OPTION */

ssize_t pwrite(int fd, const void *buf, size_t n, off_t off) {
	ssize_t num_written;
	if(int e = mlibc::sysdep_or_enosys<Pwrite>(fd, buf, n, off, &num_written); e) {
		errno = e;
		return -1;
	}
	return num_written;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("pwrite")]] ssize_t pwrite64(int fd, const void *buf, size_t n, off_t off);
#endif /* !__MLIBC_LINUX_OPTION */

ssize_t readlink(const char *__restrict path, char *__restrict buffer, size_t max_size) {
	ssize_t length;
	if(int e = mlibc::sysdep_or_enosys<Readlink>(path, buffer, max_size, &length); e) {
		errno = e;
		return -1;
	}
	return length;
}

ssize_t readlinkat(int dirfd, const char *__restrict path, char *__restrict buffer, size_t max_size) {
	ssize_t length;
	if(int e = mlibc::sysdep_or_enosys<Readlinkat>(dirfd, path, buffer, max_size, &length); e) {
		errno = e;
		return -1;
	}
	return length;
}

int rmdir(const char *path) {
	if(int e = mlibc::sysdep_or_enosys<Rmdir>(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setegid(gid_t egid) {
	if(int e = mlibc::sysdep_or_enosys<SetEgid>(egid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int seteuid(uid_t euid) {
	if(int e = mlibc::sysdep_or_enosys<SetEuid>(euid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setgid(gid_t gid) {
	if(int e = mlibc::sysdep_or_enosys<SetGid>(gid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setpgid(pid_t pid, pid_t pgid) {
	if(int e = mlibc::sysdep_or_enosys<SetPgid>(pid, pgid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t setpgrp(void) {
	return setpgid(0, 0);
}

int setregid(gid_t rgid, gid_t egid) {
	if(int e = mlibc::sysdep_or_enosys<SetRegid>(rgid, egid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setreuid(uid_t ruid, uid_t euid) {
	if(int e = mlibc::sysdep_or_enosys<SetReuid>(ruid, euid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t setsid(void) {
	pid_t sid;
	if(int e = mlibc::sysdep_or_enosys<SetSid>(&sid); e) {
		errno = e;
		return -1;
	}
	return sid;
}

int setuid(uid_t uid) {
	if(int e = mlibc::sysdep_or_enosys<SetUid>(uid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void swab(const void *__restrict _src, void *__restrict _dest, ssize_t n) {
	auto src = reinterpret_cast<const char *__restrict>(_src);
	auto dest = reinterpret_cast<char *__restrict>(_dest);
	for (ssize_t i = 0; i < n && n - i > 1; i += 2) {
		dest[i] = src[i + 1];
		dest[i + 1] = src[i];
	}
}

int symlink(const char *target_path, const char *link_path) {
	if(int e = mlibc::sysdep_or_enosys<Symlink>(target_path, link_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int symlinkat(const char *target_path, int dirfd, const char *link_path) {
	if(int e = mlibc::sysdep_or_enosys<Symlinkat>(target_path, dirfd, link_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void sync(void) {
	if constexpr (!mlibc::IsImplemented<Sync>) {
		MLIBC_MISSING_SYSDEP();
	} else {
		mlibc::sysdep_or_panic<Sync>();
	}
}

namespace {

constexpr long SYSCONF_NO_LIMIT = -1;

constexpr auto staticSysconfValues = [] {
	auto entries = std::to_array<std::pair<int, long>>({
	    // {_SC_AIO_LISTIO_MAX, AIO_LISTIO_MAX},
	    // {_SC_AIO_MAX, AIO_MAX},
	    // {_SC_AIO_PRIO_DELTA_MAX, AIO_PRIO_DELTA_MAX},
	    {_SC_ARG_MAX,
	     2097152}, // On linux, it is defined to 2097152 in most cases, so define it to be 2097152
	    {_SC_ATEXIT_MAX, SYSCONF_NO_LIMIT},
	    {_SC_BC_BASE_MAX, BC_BASE_MAX},
	    {_SC_BC_DIM_MAX, BC_DIM_MAX},
	    {_SC_BC_SCALE_MAX, BC_SCALE_MAX},
	    {_SC_BC_STRING_MAX, BC_STRING_MAX},
	    // {_SC_CHILD_MAX, SYSCONF_NO_LIMIT},
	    {_SC_COLL_WEIGHTS_MAX, COLL_WEIGHTS_MAX},
	    {_SC_DELAYTIMER_MAX, INT_MAX},
	    {_SC_EXPR_NEST_MAX, EXPR_NEST_MAX},
	    {_SC_HOST_NAME_MAX, HOST_NAME_MAX},
	    {_SC_IOV_MAX, IOV_MAX},
	    {_SC_LINE_MAX, LINE_MAX},
	    {_SC_LOGIN_NAME_MAX, LOGIN_NAME_MAX},
	    {_SC_NGROUPS_MAX, NGROUPS_MAX},
	    // {_SC_MQ_OPEN_MAX, MQ_OPEN_MAX},
	    // {_SC_MQ_PRIO_MAX, MQ_PRIO_MAX},
	    {_SC_OPEN_MAX, OPEN_MAX},
	    {_SC_PAGE_SIZE, mlibc::page_size},
	    {_SC_PAGESIZE, mlibc::page_size},
	    {_SC_THREAD_DESTRUCTOR_ITERATIONS, PTHREAD_DESTRUCTOR_ITERATIONS},
	    {_SC_THREAD_KEYS_MAX, PTHREAD_KEYS_MAX},
	    {_SC_THREAD_STACK_MIN, PTHREAD_STACK_MIN},
	    {_SC_THREAD_THREADS_MAX, SYSCONF_NO_LIMIT},
	    {_SC_RE_DUP_MAX, RE_DUP_MAX},
	    // {_SC_RTSIG_MAX, RTSIG_MAX},
	    // {_SC_SEM_NSEMS_MAX, SEM_NSEMS_MAX},
	    // {_SC_SEM_VALUE_MAX, SEM_VALUE_MAX},
	    // {_SC_SIGQUEUE_MAX, SIGQUEUE_MAX},
	    // {_SC_STREAM_MAX, STREAM_MAX},
	    // {_SC_SYMLOOP_MAX, SYMLOOP_MAX},
	    // {_SC_TIMER_MAX, TIMER_MAX},
	    // {_SC_TTY_NAME_MAX, TTY_NAME_MAX},
	    {_SC_TZNAME_MAX, TZNAME_MAX},

	    {_SC_ADVISORY_INFO, _POSIX_ADVISORY_INFO},
	    {_SC_BARRIERS, _POSIX_BARRIERS},
	    {_SC_ASYNCHRONOUS_IO, _POSIX_ASYNCHRONOUS_IO},

	    {_SC_CLOCK_SELECTION, _POSIX_CLOCK_SELECTION},
	    // {_SC_CPUTIME, _POSIX_CPUTIME},
	    // {_SC_DEVICE_CONTROL, _POSIX_DEVICE_CONTROL},
	    {_SC_FSYNC, _POSIX_FSYNC},
	    {_SC_IPV6, _POSIX_IPV6},
	    {_SC_JOB_CONTROL, _POSIX_JOB_CONTROL},
	    {_SC_MAPPED_FILES, _POSIX_MAPPED_FILES},
	    {_SC_MEMLOCK, _POSIX_MEMLOCK},
	    {_SC_MEMLOCK_RANGE, _POSIX_MEMLOCK_RANGE},
	    {_SC_MEMORY_PROTECTION, _POSIX_MEMORY_PROTECTION},
	    // {_SC_MESSAGE_PASSING, _POSIX_MESSAGE_PASSING},
	    {_SC_MONOTONIC_CLOCK, _POSIX_MONOTONIC_CLOCK},
	    // {_SC_PRIORITIZED_IO, _POSIX_PRIORITIZED_IO},
	    // {_SC_PRIORITY_SCHEDULING, _POSIX_PRIORITY_SCHEDULING},
	    // {_SC_RAW_SOCKETS, _POSIX_RAW_SOCKETS},
	    {_SC_READER_WRITER_LOCKS, _POSIX_READER_WRITER_LOCKS},
	    {_SC_REALTIME_SIGNALS, _POSIX_REALTIME_SIGNALS},
	    {_SC_REGEXP, _POSIX_REGEXP},
	    {_SC_SAVED_IDS, _POSIX_SAVED_IDS},
	    {_SC_SEMAPHORES, _POSIX_SEMAPHORES},
	    {_SC_SHARED_MEMORY_OBJECTS, _POSIX_SHARED_MEMORY_OBJECTS},
	    {_SC_SHELL, _POSIX_SHELL},
	    {_SC_SPAWN, _POSIX_SPAWN},
	    {_SC_SPIN_LOCKS, _POSIX_SPIN_LOCKS},
	    // {_SC_SPORADIC_SERVER, _POSIX_SPORADIC_SERVER},
	    {_SC_SS_REPL_MAX, _POSIX_SS_REPL_MAX},
	    {_SC_SYNCHRONIZED_IO, _POSIX_SYNCHRONIZED_IO},
	    {_SC_THREAD_ATTR_STACKADDR, _POSIX_THREAD_ATTR_STACKADDR},
	    {_SC_THREAD_ATTR_STACKSIZE, _POSIX_THREAD_ATTR_STACKSIZE},
	    // {_SC_THREAD_CPUTIME, _POSIX_THREAD_CPUTIME},
	    // {_SC_THREAD_PRIO_INHERIT, _POSIX_THREAD_PRIO_INHERIT},
	    // {_SC_THREAD_PRIO_PROTECT, _POSIX_THREAD_PRIO_PROTECT},
	    // {_SC_THREAD_PRIORITY_SCHEDULING, _POSIX_THREAD_PRIORITY_SCHEDULING},
	    // {_SC_THREAD_PROCESS_SHARED, _POSIX_THREAD_PROCESS_SHARED},
	    // {_SC_THREAD_ROBUST_PRIO_INHERIT, _POSIX_THREAD_ROBUST_PRIO_INHERIT},
	    // {_SC_THREAD_ROBUST_PRIO_PROTECT, _POSIX_THREAD_ROBUST_PRIO_PROTECT},
	    {_SC_THREAD_SAFE_FUNCTIONS, _POSIX_THREAD_SAFE_FUNCTIONS},
	    // {_SC_THREAD_SPORADIC_SERVER, _POSIX_THREAD_SPORADIC_SERVER},
	    {_SC_THREADS, _POSIX_THREADS},
	    {_SC_TIMEOUTS, _POSIX_TIMEOUTS},
	    {_SC_TIMERS, _POSIX_TIMERS},
	    // {_SC_TYPED_MEMORY_OBJECTS, _POSIX_TYPED_MEMORY_OBJECTS},
	    {_SC_VERSION, _POSIX_VERSION},
	    // {_SC_V8_ILP32_OFF32, _POSIX_V8_ILP32_OFF32},
	    // {_SC_V8_ILP32_OFFBIG, _POSIX_V8_ILP32_OFFBIG},
	    // {_SC_V8_LP64_OFF64, _POSIX_V8_LP64_OFF64},
	    // {_SC_V8_LPBIG_OFFBIG, _POSIX_V8_LPBIG_OFFBIG},
	    // {_SC_V7_ILP32_OFF32, _POSIX_V7_ILP32_OFF32},
	    // {_SC_V7_ILP32_OFFBIG, _POSIX_V7_ILP32_OFFBIG},
	    // {_SC_V7_LP64_OFF64, _POSIX_V7_LP64_OFF64},
	    // {_SC_V7_LPBIG_OFFBIG, _POSIX_V7_LPBIG_OFFBIG},
	    // {_SC_2_C_BIND, _POSIX2_C_BIND},
	    // {_SC_2_C_DEV, _POSIX2_C_DEV},
	    // {_SC_2_CHAR_TERM, _POSIX2_CHAR_TERM},
	    // {_SC_2_FORT_RUN, _POSIX2_FORT_RUN},
	    // {_SC_2_LOCALEDEF, _POSIX2_LOCALEDEF},
	    // {_SC_2_SW_DEV, _POSIX2_SW_DEV},
	    // {_SC_2_UPE, _POSIX2_UPE},
	    {_SC_2_VERSION, _POSIX2_VERSION},
	    // {_SC_XOPEN_CRYPT, _XOPEN_CRYPT},
	    {_SC_XOPEN_CRYPT, -1},
	    // {_SC_XOPEN_ENH_I18N, _XOPEN_ENH_I18N},
	    // {_SC_XOPEN_REALTIME, _XOPEN_REALTIME},
	    // {_SC_XOPEN_REALTIME_THREADS, _XOPEN_REALTIME_THREADS},
	    // {_SC_XOPEN_SHM, _XOPEN_SHM},
	    {_SC_XOPEN_UNIX, _XOPEN_UNIX},
	    // {_SC_XOPEN_UUCP, _XOPEN_UUCP},
	    {_SC_XOPEN_VERSION, _XOPEN_VERSION},

	    {_SC_TIMERS, _POSIX_TIMERS},
	    {_SC_VERSION, _POSIX_VERSION},
	    {_SC_2_VERSION, _POSIX2_VERSION},
	    {_SC_XOPEN_VERSION, _XOPEN_VERSION},
	});

	std::array<std::optional<long>, __MLIBC_SC_MAX> res{};

	for (auto [name, value] : entries)
		res[name] = value;

	return res;
}();

} // namespace

long sysconf(int number) {
	if constexpr (mlibc::IsImplemented<Sysconf>) {
		long ret = 0;

		int e = mlibc::sysdep_or_enosys<Sysconf>(number, &ret);

		if(e && e != EINVAL) {
			errno = e;
			return -1;
		}

		if(e != EINVAL) {
			return ret;
		}
	}

	if (number >= 0 && static_cast<size_t>(number) < staticSysconfValues.size())
		if (auto val = staticSysconfValues[number]; val)
			return *val;

	/* default return values, if not overriden by sysdep */
	switch(number) {
		case _SC_PHYS_PAGES:
#if __MLIBC_LINUX_OPTION
			if constexpr (mlibc::IsImplemented<Sysinfo>) {
				struct sysinfo info{};
				if(mlibc::sysdep_or_enosys<Sysinfo>(&info) == 0)
					return info.totalram * info.mem_unit / mlibc::page_size;
			}
#endif
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_PHYS_PAGES) returns fallback value 1024\e[39m" << frg::endlog;
			return 1024;
		case _SC_AVPHYS_PAGES:
#if __MLIBC_LINUX_OPTION
			if constexpr (mlibc::IsImplemented<Sysinfo>) {
				struct sysinfo info{};
				if(mlibc::sysdep_or_panic<Sysinfo>(&info) == 0)
					return info.freeram * info.mem_unit / mlibc::page_size;
			}
#endif
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_AVPHYS_PAGES) returns fallback value 1024\e[39m" << frg::endlog;
			return 1024;
		case _SC_NPROCESSORS_ONLN:
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_NPROCESSORS_ONLN) returns fallback value 1\e[39m" << frg::endlog;
			return 1;
		case _SC_GETPW_R_SIZE_MAX:
			return NSS_BUFLEN_PASSWD;
		case _SC_GETGR_R_SIZE_MAX:
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_GETGR_R_SIZE_MAX) returns fallback value 1024\e[39m" << frg::endlog;
			return 1024;
		case _SC_CHILD_MAX:
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_CHILD_MAX) returns fallback value 25\e[39m" << frg::endlog;
			// On linux, it is defined to 25 in most cases, so define it to be 25
			return 25;
		case _SC_CLK_TCK:
			// TODO: This should be obsolete?
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_CLK_TCK) is obsolete and returns arbitrary value 1000000\e[39m" << frg::endlog;
			return 1000000;
		case _SC_NPROCESSORS_CONF:
			// TODO: actually return a proper value for _SC_NPROCESSORS_CONF
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_NPROCESSORS_CONF) unconditionally returns fallback value 1\e[39m" << frg::endlog;
			return 1;
		case _SC_SYMLOOP_MAX:
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_SYMLOOP_MAX) unconditionally returns fallback value 8\e[39m" << frg::endlog;
			return 8;
		default:
			mlibc::infoLogger() << "\e[31mmlibc: sysconf() call is not implemented, number: " << number << "\e[39m" << frg::endlog;
			errno = EINVAL;
			return -1;
	}
}

pid_t tcgetpgrp(int fd) {
	int pgrp, scratch;
	if(int e = mlibc::sysdep_or_enosys<Ioctl>(fd, TIOCGPGRP, &pgrp, &scratch); e) {
		errno = e;
		return -1;
	}

	return pgrp;
}

int tcsetpgrp(int fd, pid_t pgrp) {
	int scratch;
	if(int e = mlibc::sysdep_or_enosys<Ioctl>(fd, TIOCSPGRP, &pgrp, &scratch); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int truncate(const char *path, off_t length) {
	if(int e = mlibc::sysdep_or_enosys<Truncate>(path, length); e) {
		errno = e;
		return -1;
	}

	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("truncate")]] int truncate64(const char *, off64_t);
#endif /* !__MLIBC_LINUX_OPTION */

char *ttyname(int fd) {
	const size_t size = 128;
	static thread_local char buf[size];
	if(int e = mlibc::sysdep_or_enosys<Ttyname>(fd, buf, size); e) {
		errno = e;
		return nullptr;
	}
	return buf;
}

int ttyname_r(int fd, char *buf, size_t size) {
	if(int e = mlibc::sysdep_or_enosys<Ttyname>(fd, buf, size); e) {
		return e;
	}
	return 0;
}

int unlink(const char *path) {
	if(int e = mlibc::sysdep_or_enosys<Unlinkat>(AT_FDCWD, path, 0); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int unlinkat(int fd, const char *path, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Unlinkat>(fd, path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getpagesize() {
	return mlibc::page_size;
}

int getdtablesize(void){
	return sysconf(_SC_OPEN_MAX);
}

// Code taken from musl
// GLIBC extension for stdin/stdout
char *getpass(const char *prompt) {
	int fdin, fdout;
	struct termios s, t;
	ssize_t l;
	static char password[128];

	if((fdin = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC)) < 0) {
		fdin = STDIN_FILENO;
		fdout = STDOUT_FILENO;
	} else {
		fdout = fdin;
	}

	tcgetattr(fdin, &t);
	s = t;
	t.c_lflag &= ~(ECHO | ISIG);
	t.c_lflag |= ICANON;
	t.c_iflag &= ~(INLCR | IGNCR);
	t.c_iflag |= ICRNL;
	tcsetattr(fdin, TCSAFLUSH, &t);
	tcdrain(fdin);

	dprintf(fdout, "%s", prompt);

	l = read(fdin, password, sizeof password);
	if(l >= 0) {
		if((l > 0 && password[l - 1] == '\n') || l == sizeof password)
			l--;
		password[l] = 0;
	}

	tcsetattr(fdin, TCSAFLUSH, &s);

	dprintf(fdout, "\n");
	if(fdin != STDIN_FILENO) {
		close(fdin);
	}

	return l < 0 ? nullptr : password;
}

char *get_current_dir_name(void) {
	char *pwd;
	struct stat dotstat, pwdstat;

	pwd = getenv ("PWD");
	if(pwd != nullptr && stat(".", &dotstat) == 0
		&& stat(pwd, &pwdstat) == 0 && pwdstat.st_dev == dotstat.st_dev
		&& pwdstat.st_ino == dotstat.st_ino)
		/* The PWD value is correct.  Use it.  */
		return strdup(pwd);

	return getcwd((char *) nullptr, 0);
}

// This is a Linux extension
pid_t gettid(void) {
	if constexpr (!mlibc::IsImplemented<GetTid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_gettid()");
	} else {
		return mlibc::sysdep_or_panic<GetTid>();
	}
}

int getentropy(void *buffer, size_t length) {
	if(length > GETENTROPY_MAX) {
		errno = EINVAL;
		return -1;
	}
	if(int e = mlibc::sysdep_or_enosys<GetEntropy>(buffer, length); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
	ssize_t bytes_written;
	if(int e = mlibc::sysdep<Write>(fd, buf, count, &bytes_written); e) {
		errno = e;
		return (ssize_t)-1;
	}
	return bytes_written;
}

ssize_t read(int fd, void *buf, size_t count) {
	ssize_t bytes_read;
	if(int e = mlibc::sysdep<Read>(fd, buf, count, &bytes_read); e) {
		errno = e;
		return (ssize_t)-1;
	}
	return bytes_read;
}

off_t lseek(int fd, off_t offset, int whence) {
	off_t new_offset;
	if(int e = mlibc::sysdep<Seek>(fd, offset, whence, &new_offset); e) {
		errno = e;
		return (off_t)-1;
	}
	return new_offset;
}

off64_t lseek64(int fd, off64_t offset, int whence) {
	off64_t new_offset;
	if(int e = mlibc::sysdep<Seek>(fd, offset, whence, &new_offset); e) {
		errno = e;
		return (off64_t)-1;
	}
	return new_offset;
}

int close(int fd) {
	if(int e = mlibc::sysdep<Close>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

unsigned int sleep(unsigned int secs) {
	time_t seconds = secs;
	long nanos = 0;
	if constexpr (!mlibc::IsImplemented<Sleep>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}
	mlibc::sysdep_or_panic<Sleep>(&seconds, &nanos);
	return seconds;
}

// In contrast to sleep() this functions returns 0/-1 on success/failure.
int usleep(useconds_t usecs) {
	constexpr long usec_per_sec = 1'000'000;

	time_t seconds = usecs / usec_per_sec;
	long nanos = (usecs % usec_per_sec) * 1000;
	if constexpr (!mlibc::IsImplemented<Sleep>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}
	if (int e = mlibc::sysdep_or_panic<Sleep>(&seconds, &nanos); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int dup(int fd) {
	int newfd;
	if(int e = mlibc::sysdep_or_enosys<Dup>(fd, 0, &newfd); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

int dup2(int fd, int newfd) {
	if(int e = mlibc::sysdep_or_enosys<Dup2>(fd, 0, newfd); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

int dup3(int oldfd, int newfd, int flags) {
	if(oldfd == newfd) {
		errno = EINVAL;
		return -1;
	}
	if(int e = mlibc::sysdep_or_enosys<Dup2>(oldfd, flags, newfd); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

pid_t _Fork(void) {
	auto self = mlibc::get_current_tcb();
	pid_t child;
	if (int e = mlibc::sysdep_or_enosys<Fork>(&child); e) {
		errno = e;
		return -1;
	}

	// update the cached TID in the TCB
	if (!child)
		__atomic_store_n(&self->tid, mlibc::refetch_tid(), __ATOMIC_RELAXED);

	return child;
}

pid_t fork(void) {
	auto self = mlibc::get_current_tcb();
	pid_t child;

	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Fork>, -1);

	auto hand = self->atforkEnd;
	while (hand) {
		if (hand->prepare)
			hand->prepare();

		hand = hand->prev;
	}

	if(int e = mlibc::sysdep_or_panic<Fork>(&child); e) {
		errno = e;
		return -1;
	}

	// update the cached TID in the TCB
	if (!child)
		__atomic_store_n(&self->tid, mlibc::refetch_tid(), __ATOMIC_RELAXED);

	hand = self->atforkBegin;
	while (hand) {
		if (!child) {
			if (hand->child)
				hand->child();
		} else {
			if (hand->parent)
				hand->parent();
		}
		hand = hand->next;
	}

	return child;
}

pid_t vfork(void) {
	auto self = mlibc::get_current_tcb();
	pid_t child;

	/*
	 * Fork handlers established using pthread_atfork(3) are not
	 * called when a multithreaded program employing the NPTL
	 * threading library calls vfork().  Fork handlers are called
	 * in this case in a program using the LinuxThreads threading
	 * library.  (See pthreads(7) for a description of Linux
	 * threading libraries.)
	 *  - vfork(2), release 5.13 of the Linux man-pages project
	 *
	 *  as a result, we call sys_fork instead of running atforks
	 */

	/* deferring to fork as implementing vfork correctly requires assembly
	 * to handle not mucking up the stack
	 */
	if(int e = mlibc::sysdep_or_enosys<Fork>(&child); e) {
		errno = e;
		return -1;
	}

	// update the cached TID in the TCB
	if (!child)
		__atomic_store_n(&self->tid, mlibc::refetch_tid(), __ATOMIC_RELAXED);

	return child;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	char *null_list[] = {
		nullptr
	};

	if(!argv)
		argv = null_list;
	if(!envp)
		envp = null_list;

	int e = mlibc::sysdep_or_enosys<Execve>(path, argv, envp);
	__ensure(e && "sys_execve() is expected to fail if it returns");
	errno = e;
	return -1;
}

gid_t getgid(void) {
	if constexpr (!mlibc::IsImplemented<GetGid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getgid()");
	} else {
		return mlibc::sysdep_or_panic<GetGid>();
	}
}

gid_t getegid(void) {
	if constexpr (!mlibc::IsImplemented<GetEgid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getegid()");
	} else {
		return mlibc::sysdep_or_panic<GetEgid>();
	}
}

uid_t getuid(void) {
	if constexpr (!mlibc::IsImplemented<GetUid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getuid()");
	} else {
		return mlibc::sysdep_or_panic<GetUid>();
	}
}

uid_t geteuid(void) {
	if constexpr (!mlibc::IsImplemented<GetEuid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_geteuid()");
	} else {
		return mlibc::sysdep_or_panic<GetEuid>();
	}
}

pid_t getpid(void) {
	if constexpr (!mlibc::IsImplemented<GetPid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getpid()");
	} else {
		return mlibc::sysdep_or_panic<GetPid>();
	}
}

pid_t getppid(void) {
	if constexpr (!mlibc::IsImplemented<GetPpid>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getppid()");
	} else {
		return mlibc::sysdep_or_panic<GetPpid>();
	}
}

int access(const char *path, int mode) {
	if(int e = mlibc::sysdep_or_enosys<Access>(path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

namespace {
	FILE *user_shell_global_file; // Used by setusershell/getusershell/endusershell.

	bool user_shell_open_global_file() {
		if(!user_shell_global_file) {
			user_shell_global_file = fopen("/etc/shells", "r");
			if(!user_shell_global_file) {
				// if the file cannot be opened, we need to pretend one exists with
				// these shells:
				static char shells[] = "/bin/sh\n/bin/csh\n";

				user_shell_global_file = fmemopen(shells, strlen(shells), "r");
				if(user_shell_global_file == nullptr)
					return false;
			}
		}

		return true;
	}

	void user_shell_close_global_file() {
		if(user_shell_global_file) {
			fclose(user_shell_global_file);
			user_shell_global_file = nullptr;
		}
	}
} // namespace

char *getusershell(void) {
	static char shell[PATH_MAX];
	if(!user_shell_open_global_file())
		return nullptr;

	if (fgets(shell, PATH_MAX, user_shell_global_file)){
		shell[strcspn(shell, "\n")] = '\0';
		return shell;
	}

	if(ferror(user_shell_global_file))
		errno = EIO;

	return nullptr;
}

void setusershell(void) {
	if(!user_shell_open_global_file())
		return;

	rewind(user_shell_global_file);
}

void endusershell(void) {
	user_shell_close_global_file();
}

int isatty(int fd) {
	if(int e = mlibc::sysdep_or_enosys<Isatty>(fd); e) {
		errno = e;
		return 0;
	}
	return 1;
}

int chroot(const char *ptr) {
	if(int e = mlibc::sysdep_or_enosys<Chroot>(ptr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int daemon(int nochdir, int noclose) {
	switch(fork()) {
		case 0: break;
		case -1: return -1;
		default: _exit(0);
	}

	if(setsid() < 0)
		return -1;

	if(!nochdir && chdir("/"))
		return -1;

	if(!noclose) {
		int fd = open("/dev/null", O_RDWR);
		if(fd < 0)
			return -1;

		bool failed = false;
		if(dup2(fd, 0) < 0 || dup2(fd, 1) < 0 || dup2(fd, 2) < 0)
			failed = true;
		if(fd > 2)
			close(fd);
		if(failed)
			return -1;
	}

	return 0;
}

char *ctermid(char *s) {
	return s ? strcpy(s, "/dev/tty") : const_cast<char *>("/dev/tty");
}

int setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	if(int e = mlibc::sysdep_or_enosys<SetResuid>(ruid, euid, suid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	if(int e = mlibc::sysdep_or_enosys<SetResgid>(rgid, egid, sgid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getdomainname(char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setdomainname(const char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	if(int e = mlibc::sysdep_or_enosys<GetResuid>(ruid, euid, suid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	if(int e = mlibc::sysdep_or_enosys<GetResgid>(rgid, egid, sgid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_BSD_OPTION
void *sbrk(intptr_t increment) {
	if(increment) {
		errno = ENOMEM;
		return (void *)-1;
	}

	void *out;
	if(int e = mlibc::sysdep_or_enosys<Brk>(&out); e) {
		errno = e;
		return (void *)-1;
	}
	return out;
}
#endif

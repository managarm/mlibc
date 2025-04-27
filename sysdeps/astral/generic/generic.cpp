#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <astral/syscall.h>
#include <astral/archctl.h>
#include <string.h>
#include <stdlib.h>
#include <asm/ioctls.h>
#include <poll.h>
#include <sys/select.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

namespace mlibc {
	int sys_setgroups(size_t size, const gid_t *list) {
		(void)size;
		(void)list;
		// TODO unstub
		return 0;
	}

	int sys_getgroups(size_t size, gid_t *list, int *ret) {
		(void)size;
		(void)list;
		(void)ret;
		// TODO unstub
		*ret = 0;
		return 0;
	}

	int sys_getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
		(void)size;
		(void)buffer;
		// TODO unstub
		mlibc::infoLogger() << "getsockopt: " << fd << " " << layer << " " << number << frg::endlog;
		return 0;
	}

	int sys_tgkill(int pid, int tid, int sig) {
		long ret;
		return syscall(SYSCALL_KILLTHREAD, &ret, pid, tid, sig);
	}

	int sys_sigpending(sigset_t *set) {
		long ret;
		return syscall(SYSCALL_SIGPENDING, &ret, (uint64_t)set);
	}

	int sys_sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal) {
		long ret;
		long err = syscall(SYSCALL_SIGTIMEDWAIT, &ret, (uint64_t)set, (uint64_t)info, (uint64_t)timeout);
		*out_signal = ret;
		return err;
	}

	int sys_sigsuspend(const sigset_t *set) {
		long ret;
		return syscall(SYSCALL_SIGSUSPEND, &ret, (uint64_t)set);
	}

	int sys_vm_protect(void *pointer, size_t size, int prot) {
		long ret;
		return syscall(SYSCALL_MPROTECT, &ret, (uint64_t)pointer, size, prot);
	}

	int sys_setuid(uid_t id) {
		long ret;
		return syscall(SYSCALL_SETUID, &ret, id);
	}

	int sys_setgid(gid_t id) {
		long ret;
		return syscall(SYSCALL_SETGID, &ret, id);
	}

	int sys_seteuid(uid_t id) {
		long ret;
		return syscall(SYSCALL_SETEUID, &ret, id);
	}

	int sys_setegid(gid_t id) {
		long ret;
		return syscall(SYSCALL_SETEGID, &ret, id);
	}

	uid_t sys_getuid() {
		uid_t r, e, s;
		sys_getresuid(&r, &e, &s);

		return r;
	}

	uid_t sys_geteuid() {
		uid_t r, e, s;
		sys_getresuid(&r, &e, &s);

		return e;
	}

	gid_t sys_getgid() {
		gid_t r, e, s;
		sys_getresgid(&r, &e, &s);

		return r;
	}

	gid_t sys_getegid() {
		gid_t r, e, s;
		sys_getresgid(&r, &e, &s);

		return e;
	}

	int sys_setresuid(uid_t _ruid, uid_t _euid, uid_t _suid) {
		long ret;
		return syscall(SYSCALL_SETRESUID, &ret, _ruid, _euid, _suid);
	}

	int sys_setresgid(gid_t _rgid, gid_t _egid, gid_t _sgid) {
		long ret;
		return syscall(SYSCALL_SETRESGID, &ret, _rgid, _egid, _sgid);
	}

	int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
		long ret;
		return syscall(SYSCALL_GETRESUID, &ret, (uint64_t)ruid, (uint64_t)euid, (uint64_t)suid);
	}

	int sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
		long ret;
		return syscall(SYSCALL_GETRESGID, &ret, (uint64_t)rgid, (uint64_t)egid, (uint64_t)sgid);
	}

	int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
		long ret;
		return syscall(SYSCALL_MKNODAT, &ret, dirfd, (uint64_t)path, mode, dev);
	}

	int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
		return sys_mknodat(dirfd, path, S_IFIFO | mode, 0);
	}

	int sys_rmdir(const char *path) {
		return sys_unlinkat(AT_FDCWD, path, AT_REMOVEDIR);
	}

	int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
		long readc;
		long error = syscall(SYSCALL_PREAD, &readc, fd, (uint64_t)buf, n, off);
		*bytes_read = readc;
		return error;
	}

	int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
		long writec;
		long error = syscall(SYSCALL_PWRITE, &writec, fd, (uint64_t)buf, n, off);
		*bytes_written = writec;
		return error;
	}
	
	#ifndef MLIBC_BUILDING_RTLD

	typedef struct {
		ino_t d_ino;
		off_t d_off;
		unsigned short d_reclen;
		unsigned char d_type;
		char d_name[1024];
	} dent_t;

	#endif

	int sys_pause() {
		long ret;
		return syscall(SYSCALL_PAUSE, &ret);
	}
	
	int sys_chroot(const char *path) {
		long ret;
		return syscall(SYSCALL_CHROOT, &ret, (uint64_t)path);
	}

	int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
		long ret;
		int len = max_addr_length;
		long error = syscall(SYSCALL_GETPEERNAME, &ret, fd, (uint64_t)addr_ptr, (uint64_t)&len);
		*actual_length = len;
		return error;
	}

	int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
		long ret;
		int len = max_addr_length;
		long error = syscall(SYSCALL_GETSOCKNAME, &ret, fd, (uint64_t)addr_ptr, (uint64_t)&len);
		*actual_length = len;
		return error;
	}

	int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
		long ret = 0;
		long err = syscall(SYSCALL_SOCKETPAIR, &ret, domain, type_and_flags, proto);
		if(err)
			return err;

		fds[0] = ret & 0xffffffff;
		fds[1] = (ret >> 32) & 0xffffffff;
		return err;
	}

	int sys_getitimer(int which, struct itimerval *curr_value) {
		long ret;
		return syscall(SYSCALL_GETITIMER, &ret, which, (uint64_t)curr_value);
	}

	int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
		long ret;
		return syscall(SYSCALL_SETITIMER, &ret, which, (uint64_t)new_value, (uint64_t)old_value);
	}

	#ifndef MLIBC_BUILDING_RTLD
	#define TTY_IOCTL_NAME 0x771101141113l
	#define TTY_NAME_MAX 32
	#define TTY_PREFIX "/dev/"

	
	int sys_ttyname(int fd, char * buffer, size_t size) {
		size_t prefixLen = strlen(TTY_PREFIX);
		if(size < TTY_NAME_MAX + prefixLen) {
			mlibc::panicLogger() << "ttyname size too small" << frg::endlog;
			__builtin_unreachable();
		}

		strcpy(buffer, TTY_PREFIX);

		int res;
		return sys_ioctl(fd, TTY_IOCTL_NAME, (void *)(buffer + prefixLen), &res);
	}
	#endif

	int sys_fsync(int fd) {
		long ret;
		return syscall(SYSCALL_FSYNC, &ret, fd);
	}

	int sys_fdatasync(int fd) {
		// TODO proper datasync syscall
		return sys_fsync(fd);
	}

	pid_t sys_getppid() {
		long ret;
		syscall(SYSCALL_GETPPID, &ret);
		return ret;
	}

	int sys_getsid(pid_t pid, pid_t *pgid) {
		long ret;
		long error = syscall(SYSCALL_GETSID, &ret, pid);
		*pgid = ret;
		return error;
	}

	int sys_getpgid(pid_t pid, pid_t *pgid) {
		long ret;
		long error = syscall(SYSCALL_GETPGID, &ret, pid);
		*pgid = ret;
		return error;
	}

	int sys_gethostname(char *buffer, size_t bufsize) {
		long ret;
		return syscall(SYSCALL_HOSTNAME, &ret, NULL, 0, (uint64_t)buffer, bufsize);
	}

	int sys_sethostname(const char *buffer, size_t bufsize) {
		long ret;
		return syscall(SYSCALL_HOSTNAME, &ret, (uint64_t)buffer, bufsize, NULL, 0);
	}

	int sys_uname(struct utsname *buf) {
		long ret;
		return syscall(SYSCALL_UNAME, &ret, (uint64_t)buf);
	}

	void sys_sync() {
		long ret;
		syscall(SYSCALL_SYNC, &ret);
	}

	#ifndef MLIBC_BUILDING_RTLD
	int sys_getentropy(void *buffer, size_t length) {
		int fd;
		int error = sys_open("/dev/urandom", O_RDONLY, 0, &fd);
		if(error)
			mlibc::panicLogger() << "/dev/urandom open error " << strerror(error) << frg::endlog;

		ssize_t bytes;
		error = sys_read(fd, buffer, length, &bytes);
		if(error) {
			mlibc::infoLogger() << "/dev/urandom read error " << strerror(error) << frg::endlog;
			return error;
		}

		sys_close(fd);
		return 0;
	}
	#endif

	int sys_kill(int pid, int signal) {
		long ret;
		return syscall(SYSCALL_KILL, &ret, pid, signal);
	}

	int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
		long ret;
		return syscall(SYSCALL_SIGPROCMASK, &ret, how, (uint64_t)set, (uint64_t)retrieve);
	}

	int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
		long ret;
		return syscall(SYSCALL_SIGALTSTACK, &ret, (uint64_t)ss, (uint64_t)oss);
	}

	#ifndef MLIBC_BUILDING_RTLD
	extern "C" void __mlibc_restorer();

	int sys_sigaction(int sig, const struct sigaction *__restrict act,
		struct sigaction *__restrict oldact) {
		long ret;

		struct sigaction newAction;
		if(act)
			memcpy(&newAction, act, sizeof(struct sigaction));

		if(act && (newAction.sa_flags & SA_RESTORER) == 0) {
			newAction.sa_restorer = __mlibc_restorer;
			newAction.sa_flags |= SA_RESTORER;
		}

		return syscall(SYSCALL_SIGACTION, &ret, sig, act ? (uint64_t)&newAction : 0, (uint64_t)oldact);
	}

	int sys_ptsname(int fd, char *buffer, size_t length) {
		int index;
		int tmp;
		if(int e = sys_ioctl(fd, TIOCGPTN, &index, &tmp); e)
			return e;

		if((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
			return ERANGE;
		}

		return 0;
	}
	#endif

	int sys_setpgid(pid_t pid, pid_t pgid) {
		long ret;
		return syscall(SYSCALL_SETPGID, &ret, pid, pgid);
	}

	int sys_setsid(pid_t *out) {
		long ret;
		long error = syscall(SYSCALL_SETSID, &ret);
		*out = ret;
		return error;
	}

	int sys_futex_tid() {
		long ret;
		syscall(SYSCALL_GETTID, &ret);
		return ret;
	}

	#ifndef MLIBC_BUILDING_RTLD

	[[noreturn]] void sys_thread_exit() {
		syscall(SYSCALL_THREADEXIT, nullptr);
		__builtin_unreachable();
	}

	extern "C" void __mlibc_thread_entry();

	int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
		(void)tcb;
		long ret;
		long err = syscall(SYSCALL_NEWTHREAD, &ret, (uintptr_t)__mlibc_thread_entry, (uintptr_t)stack);
		*pid_out = ret;
		return err;
	}

	#endif

	int sys_listen(int fd, int backlog) {
		long ret;
		return syscall(SYSCALL_LISTEN, &ret, fd, backlog);
	}

	int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
		long ret;
		long error = syscall(SYSCALL_ACCEPT, &ret, fd, (uint64_t)addr_ptr, (uint64_t)addr_length, flags);
		*newfd = ret;
		return error;
	}

	int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
		long ret;
		return syscall(SYSCALL_CONNECT, &ret, fd, (uint64_t)addr_ptr, addr_length);
	}

	int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
		long ret;
		long err = syscall(SYSCALL_RECVMSG, &ret, fd, (uint64_t)hdr, flags);
		*length = ret;
		return err;
	}

	int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
		long ret;
		return syscall(SYSCALL_SETSOCKOPT, &ret, fd, layer, number, (uint64_t)buffer, size);
	}

	int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
		long ret;
		long err = syscall(SYSCALL_SENDMSG, &ret, fd, (uint64_t)hdr, flags);
		*length = ret;
		return err;
	}
	
	int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
		long ret;
		return syscall(SYSCALL_BIND, &ret, fd, (uint64_t)addr_ptr, addr_length);
	}

	int sys_socket(int family, int type, int protocol, int *fd) {
		long ret;
		long err = syscall(SYSCALL_SOCKET, &ret, family, type, protocol);
		*fd = ret;
		return err;
	}

	int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
		long ret;
		return syscall(SYSCALL_RENAMEAT, &ret, olddirfd, (uint64_t)old_path, newdirfd, (uint64_t)new_path);
	}

	int sys_rename(const char *path, const char *new_path) {
		return sys_renameat(AT_FDCWD, path, AT_FDCWD, new_path);
	}

	int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
		long ret;
		return syscall(SYSCALL_UTIMENSAT, &ret, dirfd, (uint64_t)pathname, (uint64_t)times, flags);
	}

	int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
		long ret;
		return syscall(SYSCALL_FCHOWNAT, &ret, dirfd, (uint64_t)pathname, owner, group, flags);
	}

	int sys_mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data) {
		long ret;
		return syscall(SYSCALL_MOUNT, &ret, (uint64_t)source, (uint64_t)target, (uint64_t)fstype, flags, (uint64_t)data);
	}

	int sys_ftruncate(int fd, size_t size) {
		long ret;
		return syscall(SYSCALL_FTRUNCATE, &ret, fd, size);
	}

	int sys_sleep(time_t *secs, long *nanos) {
		struct timespec ts;
		ts.tv_sec = *secs;
		ts.tv_nsec = *nanos;
		long ret;
		long err = syscall(SYSCALL_NANOSLEEP, &ret, (uintptr_t)&ts, (uintptr_t)&ts);
		*secs = ts.tv_sec;
		*nanos = ts.tv_nsec;
		return err;
	}

	int sys_tcgetattr(int fd, struct termios *attr){
		int res;
		return sys_ioctl(fd, TCGETS, (void *)attr, &res);
	}

	int sys_tcsetattr(int fd, int act, const struct termios *attr){
		(void)act;
		int res;
		return sys_ioctl(fd, TCSETS, (void *)attr, &res);
	}

	int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
		long ret;
		long error = syscall(SYSCALL_POLL, &ret, (uint64_t)fds, count, timeout);
		*num_events = ret;
		return error;
	}

	int sys_ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
		long ret;
		long error = syscall(SYSCALL_PPOLL, &ret, (uint64_t)fds, nfds, (uint64_t)timeout, (uint64_t)sigmask);
		*num_events = ret;
		return error;
	}

#ifndef MLIBC_BUILDING_RTLD
	int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
		pollfd *fds = (pollfd *)malloc(num_fds * sizeof(pollfd));

		if(fds == NULL)
			return ENOMEM;

		int actual_count = 0;

		for(int fd = 0; fd < num_fds; ++fd) {
			short events = 0;
			if(read_set && FD_ISSET(fd, read_set)) {
				events |= POLLIN;
			}

			if(write_set && FD_ISSET(fd, write_set)) {
				events |= POLLOUT;
			}

			if(except_set && FD_ISSET(fd, except_set)) {
				events |= POLLPRI;
			}

			if(events) {
				fds[actual_count].fd = fd;
				fds[actual_count].events = events;
				fds[actual_count].revents = 0;
				actual_count++;
			}
		}

		int num;
		int err = sys_ppoll(fds, actual_count, timeout, sigmask, &num);

		if(err) {
			free(fds);
			return err;
		}

		#define READ_SET_POLLSTUFF (POLLIN | POLLHUP | POLLERR)
		#define WRITE_SET_POLLSTUFF (POLLOUT | POLLERR)
		#define EXCEPT_SET_POLLSTUFF (POLLPRI)

		int return_count = 0;
		for(int fd = 0; fd < actual_count; ++fd) {
			int events = fds[fd].events;
			if((events & POLLIN) && (fds[fd].revents & READ_SET_POLLSTUFF) == 0) {
				FD_CLR(fds[fd].fd, read_set);
				events &= ~POLLIN;
			}

			if((events & POLLOUT) && (fds[fd].revents & WRITE_SET_POLLSTUFF) == 0) {
				FD_CLR(fds[fd].fd, write_set);
				events &= ~POLLOUT;
			}

			if((events & POLLPRI) && (fds[fd].revents & EXCEPT_SET_POLLSTUFF) == 0) {
				FD_CLR(fds[fd].fd, except_set);
				events &= ~POLLPRI;
			}

			if(events)
				return_count++;
		}
		*num_events = return_count;
		free(fds);
		return 0;
	}
#endif

	int sys_umask(mode_t mode, mode_t *old) {
		long ret;
		long error = syscall(SYSCALL_UMASK, &ret, mode);
		*old = ret;
		return error;
	}

	int sys_fchmod(int fd, mode_t mode) {
		long ret;
		return syscall(SYSCALL_FCHMOD, &ret, fd, mode);
	}

	int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
		long ret;
		return syscall(SYSCALL_FCHMODAT, &ret, fd, (uint64_t)pathname, mode, flags);
	}

	int sys_chmod(const char *pathname, mode_t mode) {
		return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
	}

	int sys_readlinkat(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length) {
		long ret;
		long error = syscall(SYSCALL_READLINKAT, &ret, dirfd, (uint64_t)path, (uint64_t)buffer, max_size);
		*length = ret;
		return error;
	}

	int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) {
		return sys_readlinkat(AT_FDCWD, path, buffer, max_size, length);
	}

	static int dolink(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags, int type) {
		long ret;
		return syscall(SYSCALL_LINKAT, &ret, oldfd, (uint64_t)oldpath, newfd, (uint64_t)newpath, flags, type);
	}

	int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
		return dolink(olddirfd, old_path, newdirfd, new_path, flags, 0);
	}

	int sys_link(const char *old_path, const char *new_path) {
		return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
	}
	
	int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
		return dolink(AT_FDCWD, target_path, dirfd, link_path, 0, 1);
	}

	int sys_symlink(const char *target_path, const char *link_path) {
		return sys_symlinkat(target_path, AT_FDCWD, link_path);
	}

	int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
		long ret;
		return syscall(SYSCALL_MKDIRAT, &ret, dirfd, (uint64_t)path, mode);
	}

	int sys_mkdir(const char *path, mode_t mode) {
		return sys_mkdirat(AT_FDCWD, path, mode);
	}

	int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
		long ret;
		long err = syscall(SYSCALL_IOCTL, &ret, fd, request, (uint64_t)arg);
		*result = ret;
		return err;
	}

	int sys_unlinkat(int fd, const char *path, int flags) {
		long ret;
		return syscall(SYSCALL_UNLINKAT, &ret, fd, (uint64_t)path, flags);
	}

	int sys_faccessat(int dirfd, const char *pathname, int mode, int flags){
		long ret;
		return syscall(SYSCALL_FACCESSAT, &ret, dirfd, (uint64_t)pathname, mode, flags);
	}

	int sys_access(const char *path, int mode){
		return sys_faccessat(AT_FDCWD, path, mode, 0);
	}

	int sys_pipe(int *fds, int flags) {
		long ret = 0;
		long err = syscall(SYSCALL_PIPE2, &ret, flags);
		if(err)
			return err;

		fds[0] = ret & 0xffffffff;
		fds[1] = (ret >> 32) & 0xffffffff;
		return err;
	}

	int sys_chdir(const char *path) {
		long ret;
		return syscall(SYSCALL_CHDIR, &ret, (uint64_t)path);
	}

	int sys_fchdir(int fd) {
		long ret;
		return syscall(SYSCALL_FCHDIR, &ret, fd);
	}

	int sys_fcntl(int fd, int request, va_list args, int *result) {
		long arg = va_arg(args, uint64_t);
		long ret;
		long err = syscall(SYSCALL_FCNTL, &ret, fd, request, arg);
		*result = ret;
		return err;
	}

	int sys_dup(int fd, int flags, int *newfd) {
		(void)flags;
		long ret;
		long err = syscall(SYSCALL_DUP, &ret, fd);
		*newfd = ret;
		return err;
	}

	int sys_dup2(int fd, int flags, int newfd) {
		(void)flags;
		long ret;
		return syscall(SYSCALL_DUP2, &ret, fd, newfd);
	}

	int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
		long ret;
		long err = syscall(SYSCALL_GETDENTS, &ret, handle, (uint64_t)buffer, max_size);
		if(err)
			return err;
		*bytes_read = ret;
		return err;
	}

	int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
		(void)ru;
		long ret;
		long err = syscall(SYSCALL_WAITPID, &ret, pid, (uint64_t)status, flags);
		*ret_pid = ret;
		return err;
	}

	int sys_execve(const char *path, char *const argv[], char *const envp[]) {
		long ret;
		return syscall(SYSCALL_EXECVE, &ret, (uint64_t)path, (uint64_t)argv, (uint64_t)envp);
	}

	int sys_fork(pid_t *pid) {
		long ret = 0;
		long error = syscall(SYSCALL_FORK, &ret);
		*pid = ret;
		return error;
	}

	int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
		long ret;
		switch (fsfdt) {
			case fsfd_target::path:
				return syscall(SYSCALL_FSTATAT, &ret, AT_FDCWD, (uint64_t)path, (uint64_t)statbuf, flags);
			case fsfd_target::fd:
				return syscall(SYSCALL_FSTAT, &ret, fd, (uint64_t)statbuf);
			case fsfd_target::fd_path:
				return syscall(SYSCALL_FSTATAT, &ret, fd, (uint64_t)path, (uint64_t)statbuf, flags);
			default:
				mlibc::infoLogger() << "mlibc: stat: Unknown fsfd_target: " << (int)fsfdt << frg::endlog;
				return ENOSYS;
		}
	}

	pid_t sys_getpid() {
		long ret;
		syscall(SYSCALL_GETPID, &ret);
		return ret;
	}

	void sys_libc_log(const char *message) {
		long ret;
		syscall(SYSCALL_PRINT, &ret, (uint64_t)message);
	}
	
	[[noreturn]] void sys_libc_panic() {
		sys_libc_log("mlibc: panic");
		sys_exit(1);
	}

	[[noreturn]] void sys_exit(int status) {
		syscall(SYSCALL_EXIT, NULL, status);
		__builtin_unreachable();
	}

	int sys_tcb_set(void *pointer) {
		long r;
		return syscall(SYSCALL_ARCHCTL, &r, ARCH_CTL_FSBASE, (uint64_t)pointer);
	}

	#define FUTEX_WAIT 0
	#define FUTEX_WAKE 1

	int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
		long ret;
		return syscall(SYSCALL_FUTEX, &ret, (uint64_t)pointer, FUTEX_WAIT, expected, (uint64_t)time);
	}

	int sys_futex_wake(int *pointer) {
		long ret;
		return syscall(SYSCALL_FUTEX, &ret, (uint64_t)pointer, FUTEX_WAKE, INT_MAX, NULL);
	}

	int sys_anon_allocate(size_t size, void **pointer) {
		size += 4096 - (size % 4096);
		return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0, pointer);
	}
	int sys_anon_free(void *pointer, size_t size) {
		size += 4096 - (size % 4096);
		return sys_vm_unmap(pointer, size);
	}
	
	int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
		long ret;
		long err = syscall(SYSCALL_OPENAT, &ret, dirfd, (uint64_t)path, flags, mode);
		if(err)
			return err;
		*fd = ret;
		return 0;
	}

	int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
		return sys_openat(AT_FDCWD, pathname, flags, mode, fd);
	};

	int sys_open_dir(const char *path, int *handle) {
		return sys_open(path, O_DIRECTORY, 0, handle);
	}

	int sys_read(int fd, void *buff, size_t count, ssize_t *bytes_read) {
		long readc;
		long error = syscall(SYSCALL_READ, &readc, fd, (uint64_t)buff, count);
		*bytes_read = readc;
		return error;
	}

	int sys_write(int fd, const void *buff, size_t count, ssize_t *bytes_written) {
		long writec;
		long error = syscall(SYSCALL_WRITE, &writec, fd, (uint64_t)buff, count);
		*bytes_written = writec;
		return error;
	}

	int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
		long ret = 0;
		long error = syscall(SYSCALL_SEEK, &ret, fd, offset, whence);
		*new_offset = ret;
		return error;
	}
	
	int sys_close(int fd) {
		long r;
		return syscall(SYSCALL_CLOSE, &r, fd);
	}

	int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
		long ret;
		long err = syscall(SYSCALL_MMAP, &ret, (uint64_t)hint, size, prot, flags, fd, offset);
		*window = (void *)ret;
		return err;
	}

	int sys_vm_unmap(void *pointer, size_t size) {
		long ret;
		return syscall(SYSCALL_MUNMAP, &ret, (uintptr_t)pointer, size);
	}
	
	int sys_isatty(int fd) {
		long ret;
		return syscall(SYSCALL_ISATTY, &ret, fd);
	}

	int sys_clock_get(int clock, time_t *secs, long *nanos) {
		struct timespec ts;
		long ret;
		int err = syscall(SYSCALL_CLOCKGET, &ret, clock, (uint64_t)&ts);
		*secs = ts.tv_sec;
		*nanos = ts.tv_nsec;
		return err;
	}
} // namespace mlibc

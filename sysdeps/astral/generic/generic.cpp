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
	int Sysdeps<SetGroups>::operator()(size_t size, const gid_t *list) {
		(void)size;
		(void)list;
		// TODO unstub
		return 0;
	}

	int Sysdeps<GetGroups>::operator()(size_t size, gid_t *list, int *ret) {
		(void)size;
		(void)list;
		(void)ret;
		// TODO unstub
		*ret = 0;
		return 0;
	}

	int Sysdeps<GetSockopt>::operator()(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
		(void)size;
		(void)buffer;
		// TODO unstub
		mlibc::infoLogger() << "getsockopt: " << fd << " " << layer << " " << number << frg::endlog;
		return 0;
	}

	int Sysdeps<InetConfigured>::operator()(bool *ipv4, bool *ipv6) {
		// there is no ipv6 support in the kernel currently and no way of checking for configured interfaces
		*ipv4 = true;
		*ipv6 = false;
		return 0;
	}

	int Sysdeps<Readv>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read) {
		long r;
		int error = syscall(SYSCALL_READV, &r, fd, (uint64_t)iovs, iovc);
		*bytes_read = r;
		return error;
	}

	int Sysdeps<Writev>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written) {
		long r;
		int error = syscall(SYSCALL_WRITEV, &r, fd, (uint64_t)iovs, iovc);
		*bytes_written = r;
		return error;
	}

	int Sysdeps<Sysinfo>::operator()(struct sysinfo *info) {
		long r;
		return syscall(SYSCALL_SYSINFO, &r, (uint64_t)info);
	}

	int Sysdeps<Getcpu>::operator()(int *cpu) {
		long ret;
		// can never fail
		syscall(SYSCALL_GETCPU, &ret);
		*cpu = ret;
		return 0;
	}

	int Sysdeps<Flock>::operator()(int fd, int options) {
		long r;
		return syscall(SYSCALL_FLOCK, &r, fd, options);
	}

	int Sysdeps<Nice>::operator()(int nice, int *ret) {
		long r = -1;
		long e = syscall(SYSCALL_NICE, &r, nice);
		*ret = r;
		return e;
	}

	int Sysdeps<Shutdown>::operator()(int sockfd, int how) {
		long ret;
		return syscall(SYSCALL_SHUTDOWN, &ret, sockfd, how);
	}

	int Sysdeps<Tgkill>::operator()(int pid, int tid, int sig) {
		long ret;
		return syscall(SYSCALL_KILLTHREAD, &ret, pid, tid, sig);
	}

	int Sysdeps<Sigpending>::operator()(sigset_t *set) {
		long ret;
		return syscall(SYSCALL_SIGPENDING, &ret, (uint64_t)set);
	}

	int Sysdeps<Sigtimedwait>::operator()(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal) {
		long ret;
		long err = syscall(SYSCALL_SIGTIMEDWAIT, &ret, (uint64_t)set, (uint64_t)info, (uint64_t)timeout);
		*out_signal = ret;
		return err;
	}

	int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
		long ret;
		return syscall(SYSCALL_SIGSUSPEND, &ret, (uint64_t)set);
	}

	int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
		long ret;
		return syscall(SYSCALL_MPROTECT, &ret, (uint64_t)pointer, size, prot);
	}

	int Sysdeps<SetUid>::operator()(uid_t id) {
		long ret;
		return syscall(SYSCALL_SETUID, &ret, id);
	}

	int Sysdeps<SetGid>::operator()(gid_t id) {
		long ret;
		return syscall(SYSCALL_SETGID, &ret, id);
	}

	int Sysdeps<SetEuid>::operator()(uid_t id) {
		long ret;
		return syscall(SYSCALL_SETEUID, &ret, id);
	}

	int Sysdeps<SetEgid>::operator()(gid_t id) {
		long ret;
		return syscall(SYSCALL_SETEGID, &ret, id);
	}

	uid_t Sysdeps<GetUid>::operator()() {
		uid_t r, e, s;
		sysdep<GetResuid>(&r, &e, &s);

		return r;
	}

	uid_t Sysdeps<GetEuid>::operator()() {
		uid_t r, e, s;
		sysdep<GetResuid>(&r, &e, &s);

		return e;
	}

	gid_t Sysdeps<GetGid>::operator()() {
		gid_t r, e, s;
		sysdep<GetResgid>(&r, &e, &s);

		return r;
	}

	gid_t Sysdeps<GetEgid>::operator()() {
		gid_t r, e, s;
		sysdep<GetResgid>(&r, &e, &s);

		return e;
	}

	int Sysdeps<SetResuid>::operator()(uid_t _ruid, uid_t _euid, uid_t _suid) {
		long ret;
		return syscall(SYSCALL_SETRESUID, &ret, _ruid, _euid, _suid);
	}

	int Sysdeps<SetResgid>::operator()(gid_t _rgid, gid_t _egid, gid_t _sgid) {
		long ret;
		return syscall(SYSCALL_SETRESGID, &ret, _rgid, _egid, _sgid);
	}

	int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
		long ret;
		return syscall(SYSCALL_GETRESUID, &ret, (uint64_t)ruid, (uint64_t)euid, (uint64_t)suid);
	}

	int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
		long ret;
		return syscall(SYSCALL_GETRESGID, &ret, (uint64_t)rgid, (uint64_t)egid, (uint64_t)sgid);
	}

	int Sysdeps<Mknodat>::operator()(int dirfd, const char *path, int mode, int dev) {
		long ret;
		return syscall(SYSCALL_MKNODAT, &ret, dirfd, (uint64_t)path, mode, dev);
	}

	int Sysdeps<Mkfifoat>::operator()(int dirfd, const char *path, mode_t mode) {
		return sysdep<Mknodat>(dirfd, path, S_IFIFO | mode, 0);
	}

	int Sysdeps<Rmdir>::operator()(const char *path) {
		return sysdep<Unlinkat>(AT_FDCWD, path, AT_REMOVEDIR);
	}

	int Sysdeps<Pread>::operator()(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
		long readc;
		long error = syscall(SYSCALL_PREAD, &readc, fd, (uint64_t)buf, n, off);
		*bytes_read = readc;
		return error;
	}

	int Sysdeps<Pwrite>::operator()(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
		long writec;
		long error = syscall(SYSCALL_PWRITE, &writec, fd, (uint64_t)buf, n, off);
		*bytes_written = writec;
		return error;
	}

	int Sysdeps<GetRlimit>::operator()(int resource, struct rlimit *limit) {
		switch(resource) {
		case RLIMIT_NOFILE:
			limit->rlim_cur = RLIM_INFINITY;
			limit->rlim_max = RLIM_INFINITY;
			return 0;
		default:
			return EINVAL;
		}
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

	int Sysdeps<Pause>::operator()() {
		long ret;
		return syscall(SYSCALL_PAUSE, &ret);
	}

	int Sysdeps<Chroot>::operator()(const char *path) {
		long ret;
		return syscall(SYSCALL_CHROOT, &ret, (uint64_t)path);
	}

	int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
		long ret;
		int len = max_addr_length;
		long error = syscall(SYSCALL_GETPEERNAME, &ret, fd, (uint64_t)addr_ptr, (uint64_t)&len);
		*actual_length = len;
		return error;
	}

	int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
		long ret;
		int len = max_addr_length;
		long error = syscall(SYSCALL_GETSOCKNAME, &ret, fd, (uint64_t)addr_ptr, (uint64_t)&len);
		*actual_length = len;
		return error;
	}

	int Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds) {
		long ret = 0;
		long err = syscall(SYSCALL_SOCKETPAIR, &ret, domain, type_and_flags, proto);
		if(err)
			return err;

		fds[0] = ret & 0xffffffff;
		fds[1] = (ret >> 32) & 0xffffffff;
		return err;
	}

	int Sysdeps<GetItimer>::operator()(int which, struct itimerval *curr_value) {
		long ret;
		return syscall(SYSCALL_GETITIMER, &ret, which, (uint64_t)curr_value);
	}

	int Sysdeps<SetItimer>::operator()(int which, const struct itimerval *new_value, struct itimerval *old_value) {
		long ret;
		return syscall(SYSCALL_SETITIMER, &ret, which, (uint64_t)new_value, (uint64_t)old_value);
	}

	#ifndef MLIBC_BUILDING_RTLD
	#define TTY_IOCTL_NAME 0x771101141113l
	#define TTY_NAME_MAX 32
	#define TTY_PREFIX "/dev/"


	int Sysdeps<Ttyname>::operator()(int fd, char * buffer, size_t size) {
		size_t prefixLen = strlen(TTY_PREFIX);
		if(size < TTY_NAME_MAX + prefixLen) {
			mlibc::panicLogger() << "ttyname size too small" << frg::endlog;
			__builtin_unreachable();
		}

		strcpy(buffer, TTY_PREFIX);

		int res;
		return sysdep<Ioctl>(fd, TTY_IOCTL_NAME, (void *)(buffer + prefixLen), &res);
	}
	#endif

	int Sysdeps<Fsync>::operator()(int fd) {
		long ret;
		return syscall(SYSCALL_FSYNC, &ret, fd);
	}

	int Sysdeps<Fdatasync>::operator()(int fd) {
		// TODO proper datasync syscall
		return sysdep<Fsync>(fd);
	}

	pid_t Sysdeps<GetPpid>::operator()() {
		long ret;
		syscall(SYSCALL_GETPPID, &ret);
		return ret;
	}

	int Sysdeps<GetSid>::operator()(pid_t pid, pid_t *pgid) {
		long ret;
		long error = syscall(SYSCALL_GETSID, &ret, pid);
		*pgid = ret;
		return error;
	}

	int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
		long ret;
		long error = syscall(SYSCALL_GETPGID, &ret, pid);
		*pgid = ret;
		return error;
	}

	int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
		long ret;
		return syscall(SYSCALL_HOSTNAME, &ret, 0, 0, (uint64_t)buffer, bufsize);
	}

	int Sysdeps<SetHostname>::operator()(const char *buffer, size_t bufsize) {
		long ret;
		return syscall(SYSCALL_HOSTNAME, &ret, (uint64_t)buffer, bufsize, 0, 0);
	}

	int Sysdeps<Uname>::operator()(struct utsname *buf) {
		long ret;
		return syscall(SYSCALL_UNAME, &ret, (uint64_t)buf);
	}

	void Sysdeps<Sync>::operator()() {
		long ret;
		syscall(SYSCALL_SYNC, &ret);
	}

	#ifndef MLIBC_BUILDING_RTLD
	int Sysdeps<GetEntropy>::operator()(void *buffer, size_t length) {
		int fd;
		int error = sysdep<Open>("/dev/urandom", O_RDONLY, 0, &fd);
		if(error)
			mlibc::panicLogger() << "/dev/urandom open error " << strerror(error) << frg::endlog;

		ssize_t bytes;
		error = sysdep<Read>(fd, buffer, length, &bytes);
		if(error) {
			mlibc::infoLogger() << "/dev/urandom read error " << strerror(error) << frg::endlog;
			return error;
		}

		sysdep<Close>(fd);
		return 0;
	}
	#endif

	int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
		long ret;
		return syscall(SYSCALL_KILL, &ret, pid, signal);
	}

	int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
		long ret;
		return syscall(SYSCALL_SIGPROCMASK, &ret, how, (uint64_t)set, (uint64_t)retrieve);
	}

	int Sysdeps<Sigaltstack>::operator()(const stack_t *ss, stack_t *oss) {
		long ret;
		return syscall(SYSCALL_SIGALTSTACK, &ret, (uint64_t)ss, (uint64_t)oss);
	}

	#ifndef MLIBC_BUILDING_RTLD
	extern "C" void __mlibc_restorer();

	int Sysdeps<Sigaction>::operator()(int sig, const struct sigaction *__restrict act,
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

	int Sysdeps<Ptsname>::operator()(int fd, char *buffer, size_t length) {
		int index;
		int tmp;
		if(int e = sysdep<Ioctl>(fd, TIOCGPTN, &index, &tmp); e)
			return e;

		if((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
			return ERANGE;
		}

		return 0;
	}
	#endif

	int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
		long ret;
		return syscall(SYSCALL_SETPGID, &ret, pid, pgid);
	}

	int Sysdeps<SetSid>::operator()(pid_t *out) {
		long ret;
		long error = syscall(SYSCALL_SETSID, &ret);
		*out = ret;
		return error;
	}

	int Sysdeps<FutexTid>::operator()() {
		long ret;
		syscall(SYSCALL_GETTID, &ret);
		return ret;
	}

	pid_t Sysdeps<GetTid>::operator()() {
		long ret;
		syscall(SYSCALL_GETTID, &ret);
		return ret;
	}

	#ifndef MLIBC_BUILDING_RTLD

	[[noreturn]] void Sysdeps<ThreadExit>::operator()() {
		syscall(SYSCALL_THREADEXIT, nullptr);
		__builtin_unreachable();
	}

	extern "C" void __mlibc_thread_entry();

	int Sysdeps<Clone>::operator()(void *tcb, pid_t *pid_out, void *stack) {
		(void)tcb;
		long ret;
		long err = syscall(SYSCALL_NEWTHREAD, &ret, (uintptr_t)__mlibc_thread_entry, (uintptr_t)stack);
		*pid_out = ret;
		return err;
	}

	#endif

	int Sysdeps<Listen>::operator()(int fd, int backlog) {
		long ret;
		return syscall(SYSCALL_LISTEN, &ret, fd, backlog);
	}

	int Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
		long ret;
		long error = syscall(SYSCALL_ACCEPT, &ret, fd, (uint64_t)addr_ptr, (uint64_t)addr_length, flags);
		*newfd = ret;
		return error;
	}

	int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
		long ret;
		return syscall(SYSCALL_CONNECT, &ret, fd, (uint64_t)addr_ptr, addr_length);
	}

	int Sysdeps<MsgRecv>::operator()(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
		long ret;
		long err = syscall(SYSCALL_RECVMSG, &ret, fd, (uint64_t)hdr, flags);
		*length = ret;
		return err;
	}

	int Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
		long ret;
		return syscall(SYSCALL_SETSOCKOPT, &ret, fd, layer, number, (uint64_t)buffer, size);
	}

	int Sysdeps<MsgSend>::operator()(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
		long ret;
		long err = syscall(SYSCALL_SENDMSG, &ret, fd, (uint64_t)hdr, flags);
		*length = ret;
		return err;
	}

	int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
		long ret;
		return syscall(SYSCALL_BIND, &ret, fd, (uint64_t)addr_ptr, addr_length);
	}

	int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
		long ret;
		long err = syscall(SYSCALL_SOCKET, &ret, family, type, protocol);
		*fd = ret;
		return err;
	}

	int Sysdeps<Renameat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
		long ret;
		return syscall(SYSCALL_RENAMEAT, &ret, olddirfd, (uint64_t)old_path, newdirfd, (uint64_t)new_path);
	}

	int Sysdeps<Rename>::operator()(const char *path, const char *new_path) {
		return sysdep<Renameat>(AT_FDCWD, path, AT_FDCWD, new_path);
	}

	int Sysdeps<Utimensat>::operator()(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
		long ret;
		return syscall(SYSCALL_UTIMENSAT, &ret, dirfd, (uint64_t)pathname, (uint64_t)times, flags);
	}

	int Sysdeps<Fchownat>::operator()(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
		long ret;
		return syscall(SYSCALL_FCHOWNAT, &ret, dirfd, (uint64_t)pathname, owner, group, flags);
	}

	int Sysdeps<Mount>::operator()(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data) {
		long ret;
		return syscall(SYSCALL_MOUNT, &ret, (uint64_t)source, (uint64_t)target, (uint64_t)fstype, flags, (uint64_t)data);
	}

	int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
		long ret;
		return syscall(SYSCALL_FTRUNCATE, &ret, fd, size);
	}

	int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
		struct timespec ts;
		ts.tv_sec = *secs;
		ts.tv_nsec = *nanos;
		long ret;
		long err = syscall(SYSCALL_NANOSLEEP, &ret, (uintptr_t)&ts, (uintptr_t)&ts);
		*secs = ts.tv_sec;
		*nanos = ts.tv_nsec;
		return err;
	}

	int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr){
		int res;
		return sysdep<Ioctl>(fd, TCGETS, (void *)attr, &res);
	}

	int Sysdeps<Tcsetattr>::operator()(int fd, int act, const struct termios *attr){
		(void)act;
		int res;
		return sysdep<Ioctl>(fd, TCSETS, (void *)attr, &res);
	}

	int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
		long ret;
		long error = syscall(SYSCALL_POLL, &ret, (uint64_t)fds, count, timeout);
		*num_events = ret;
		return error;
	}

	int Sysdeps<Ppoll>::operator()(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
		long ret;
		long error = syscall(SYSCALL_PPOLL, &ret, (uint64_t)fds, nfds, (uint64_t)timeout, (uint64_t)sigmask);
		*num_events = ret;
		return error;
	}

#ifndef MLIBC_BUILDING_RTLD
	int Sysdeps<Pselect>::operator()(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
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
		int err = sysdep<Ppoll>(fds, actual_count, timeout, sigmask, &num);

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

	int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old) {
		long ret;
		long error = syscall(SYSCALL_UMASK, &ret, mode);
		*old = ret;
		return error;
	}

	int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
		long ret;
		return syscall(SYSCALL_FCHMOD, &ret, fd, mode);
	}

	int Sysdeps<Fchmodat>::operator()(int fd, const char *pathname, mode_t mode, int flags) {
		long ret;
		return syscall(SYSCALL_FCHMODAT, &ret, fd, (uint64_t)pathname, mode, flags);
	}

	int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
		return sysdep<Fchmodat>(AT_FDCWD, pathname, mode, 0);
	}

	int Sysdeps<Readlinkat>::operator()(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length) {
		long ret;
		long error = syscall(SYSCALL_READLINKAT, &ret, dirfd, (uint64_t)path, (uint64_t)buffer, max_size);
		*length = ret;
		return error;
	}

	int Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t max_size, ssize_t *length) {
		return sysdep<Readlinkat>(AT_FDCWD, path, buffer, max_size, length);
	}

	static int dolink(int oldfd, const char *oldpath, int newfd, const char *newpath, int flags, int type) {
		long ret;
		return syscall(SYSCALL_LINKAT, &ret, oldfd, (uint64_t)oldpath, newfd, (uint64_t)newpath, flags, type);
	}

	int Sysdeps<Linkat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
		return dolink(olddirfd, old_path, newdirfd, new_path, flags, 0);
	}

	int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
		return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
	}

	int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
		return dolink(AT_FDCWD, target_path, dirfd, link_path, 0, 1);
	}

	int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
		return sysdep<Symlinkat>(target_path, AT_FDCWD, link_path);
	}

	int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
		long ret;
		return syscall(SYSCALL_MKDIRAT, &ret, dirfd, (uint64_t)path, mode);
	}

	int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
		return sysdep<Mkdirat>(AT_FDCWD, path, mode);
	}

	int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
		long ret;
		long err = syscall(SYSCALL_IOCTL, &ret, fd, request, (uint64_t)arg);
		*result = ret;
		return err;
	}

	int Sysdeps<Unlinkat>::operator()(int fd, const char *path, int flags) {
		long ret;
		return syscall(SYSCALL_UNLINKAT, &ret, fd, (uint64_t)path, flags);
	}

	int Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags){
		long ret;
		return syscall(SYSCALL_FACCESSAT, &ret, dirfd, (uint64_t)pathname, mode, flags);
	}

	int Sysdeps<Access>::operator()(const char *path, int mode){
		return sysdep<Faccessat>(AT_FDCWD, path, mode, 0);
	}

	int Sysdeps<Pipe>::operator()(int *fds, int flags) {
		long ret = 0;
		long err = syscall(SYSCALL_PIPE2, &ret, flags);
		if(err)
			return err;

		fds[0] = ret & 0xffffffff;
		fds[1] = (ret >> 32) & 0xffffffff;
		return err;
	}

	int Sysdeps<Chdir>::operator()(const char *path) {
		long ret;
		return syscall(SYSCALL_CHDIR, &ret, (uint64_t)path);
	}

	int Sysdeps<Fchdir>::operator()(int fd) {
		long ret;
		return syscall(SYSCALL_FCHDIR, &ret, fd);
	}

	int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
		long arg = va_arg(args, uint64_t);
		long ret;
		long err = syscall(SYSCALL_FCNTL, &ret, fd, request, arg);
		*result = ret;
		return err;
	}

	int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
		(void)flags;
		long ret;
		long err = syscall(SYSCALL_DUP, &ret, fd);
		*newfd = ret;
		return err;
	}

	int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
		(void)flags;
		long ret;
		return syscall(SYSCALL_DUP2, &ret, fd, newfd);
	}

	int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
		long ret;
		long err = syscall(SYSCALL_GETDENTS, &ret, handle, (uint64_t)buffer, max_size);
		if(err)
			return err;
		*bytes_read = ret;
		return err;
	}

	int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
		(void)ru;
		long ret;
		long err = syscall(SYSCALL_WAITPID, &ret, pid, (uint64_t)status, flags);
		*ret_pid = ret;
		return err;
	}

	int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
		long ret;
		return syscall(SYSCALL_EXECVE, &ret, (uint64_t)path, (uint64_t)argv, (uint64_t)envp);
	}

	int Sysdeps<Fork>::operator()(pid_t *pid) {
		long ret = 0;
		long error = syscall(SYSCALL_FORK, &ret);
		*pid = ret;
		return error;
	}

	int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
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

	pid_t Sysdeps<GetPid>::operator()() {
		long ret;
		syscall(SYSCALL_GETPID, &ret);
		return ret;
	}

	void Sysdeps<LibcLog>::operator()(const char *message) {
		long ret;
		syscall(SYSCALL_PRINT, &ret, (uint64_t)message);
	}

	[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
		sysdep<LibcLog>("mlibc: panic");
		sysdep<Exit>(1);
	}

	[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
		syscall(SYSCALL_EXIT, NULL, status);
		__builtin_unreachable();
	}

	int Sysdeps<TcbSet>::operator()(void *pointer) {
		long r;
		return syscall(SYSCALL_ARCHCTL, &r, ARCH_CTL_SET_FSBASE, (uint64_t)pointer);
	}

	#define FUTEX_WAIT 0
	#define FUTEX_WAKE 1

	int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
		long ret;
		return syscall(SYSCALL_FUTEX, &ret, (uint64_t)pointer, FUTEX_WAIT, expected, (uint64_t)time);
	}

	int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
		long ret;
		return syscall(SYSCALL_FUTEX, &ret, (uint64_t)pointer, FUTEX_WAKE, all ? INT_MAX : 1, 0);
	}

	int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
		size += 4096 - (size % 4096);
		return sysdep<VmMap>(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0, pointer);
	}
	int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
		size += 4096 - (size % 4096);
		return sysdep<VmUnmap>(pointer, size);
	}

	int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
		long ret;
		long err = syscall(SYSCALL_OPENAT, &ret, dirfd, (uint64_t)path, flags, mode);
		if(err)
			return err;
		*fd = ret;
		return 0;
	}

	int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
		return sysdep<Openat>(AT_FDCWD, pathname, flags, mode, fd);
	};

	int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
		return sysdep<Open>(path, O_DIRECTORY, 0, handle);
	}

	int Sysdeps<Read>::operator()(int fd, void *buff, size_t count, ssize_t *bytes_read) {
		long readc;
		long error = syscall(SYSCALL_READ, &readc, fd, (uint64_t)buff, count);
		*bytes_read = readc;
		return error;
	}

	int Sysdeps<Write>::operator()(int fd, const void *buff, size_t count, ssize_t *bytes_written) {
		long writec;
		long error = syscall(SYSCALL_WRITE, &writec, fd, (uint64_t)buff, count);
		*bytes_written = writec;
		return error;
	}

	int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
		long ret = 0;
		long error = syscall(SYSCALL_SEEK, &ret, fd, offset, whence);
		*new_offset = ret;
		return error;
	}

	int Sysdeps<Close>::operator()(int fd) {
		long r;
		return syscall(SYSCALL_CLOSE, &r, fd);
	}

	int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
		long ret;
		long err = syscall(SYSCALL_MMAP, &ret, (uint64_t)hint, size, prot, flags, fd, offset);
		*window = (void *)ret;
		return err;
	}

	int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
		long ret;
		return syscall(SYSCALL_MUNMAP, &ret, (uintptr_t)pointer, size);
	}

	int Sysdeps<Isatty>::operator()(int fd) {
		long ret;
		return syscall(SYSCALL_ISATTY, &ret, fd);
	}

	int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
		struct timespec ts;
		long ret;
		int err = syscall(SYSCALL_CLOCKGET, &ret, clock, (uint64_t)&ts);
		*secs = ts.tv_sec;
		*nanos = ts.tv_nsec;
		return err;
	}
} // namespace mlibc


#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <bits/ensure.h>

#include <frg/mutex.hpp>
#include <mlibc/lock.hpp>
#include <mlibc/debug.hpp>

// This syslog implementation is largely taken from musl

static char log_ident[32];
static int log_options;
static int log_facility = LOG_USER;
static int log_fd = -1;
static int log_opt;
static int log_mask = 0xff;

static int use_mlibc_logger = 0;
static FutexLock __syslog_lock;

static const struct sockaddr_un log_addr {AF_UNIX, "/dev/log"};

void closelog(void) {
	frg::unique_lock<FutexLock> holder { __syslog_lock };
	close(log_fd);
	log_fd = -1;
}

static void __openlog() {
	log_fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if(log_fd >= 0) {
		int ret = connect(log_fd, (const sockaddr *)&log_addr, sizeof log_addr);
		if(ret) {
			mlibc::infoLogger() << "\e[31mmlibc: syslog: connect returned an error, falling back to infoLogger\e[39m" << frg::endlog;
			use_mlibc_logger = 1;
		}
	}
}

void openlog(const char *ident, int options, int facility) {
	frg::unique_lock<FutexLock> holder { __syslog_lock };
	if(ident) {
		size_t n = strnlen(ident, sizeof log_ident - 1);
		memcpy(log_ident, ident, n);
		log_ident[n] = 0;
	} else {
		log_ident[0] = 0;
	}
	log_options = options;
	log_facility = facility;

	if((options & LOG_NDELAY) && log_fd < 0)
		__openlog();
}

int setlogmask(int mask) {
	int old_mask = log_mask;

	log_mask = mask;

	return old_mask;
}

static void _vsyslog(int priority, const char *message, va_list ap) {
	auto is_lost_conn = [] (int e) {
		return e == ECONNREFUSED || e == ECONNRESET || e == ENOTCONN || e == EPIPE;
	};

	if(!(priority & log_mask)) {
		return;
	}

	char timebuf[16];
	time_t now;
	struct tm tm;
	char buf[1024];
	int errno_save = errno;
	int pid;
	int l, l2;
	int hlen;
	int fd;

	if(log_fd < 0)
		__openlog();

	if(use_mlibc_logger) {
		vsnprintf(buf, sizeof buf, message, ap);
		mlibc::infoLogger() << "mlibc: syslog: " << buf << frg::endlog;
		return;
	}

	if(!(priority & LOG_FACMASK))
		priority |= log_facility;

	now = time(NULL);
	gmtime_r(&now, &tm);
	strftime(timebuf, sizeof timebuf, "%b %e %T", &tm);

	pid = (log_opt & LOG_PID) ? getpid() : 0;
	l = snprintf(buf, sizeof buf, "<%d>%s %n%s%s%.0d%s: ",
		priority, timebuf, &hlen, log_ident, (pid ? "[" : ""), pid, (pid ? "]" : ""));
	errno = errno_save;
	l2 = vsnprintf(buf + l, sizeof buf - l, message, ap);
	if(l2 >= 0) {
		if(l2 >= (long int)(sizeof buf - l))
			l = sizeof buf - 1;
		else
			l += l2;
		if(buf[l - 1] != '\n')
			buf[l++] = '\n';
		if(send(log_fd, buf, l, 0) < 0 && (!is_lost_conn(errno)
		    || connect(log_fd, (const sockaddr *)&log_addr, sizeof log_addr) < 0
		    || send(log_fd, buf, l, 0) < 0)
		    && (log_opt & LOG_CONS)) {
			fd = open("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
			if(fd >= 0) {
				dprintf(fd, "%.*s", l - hlen, buf + hlen);
				close(fd);
			}
		}
		if(log_opt & LOG_PERROR)
			dprintf(STDERR_FILENO, "%.*s", l - hlen, buf + hlen);
	}
}

void syslog(int priority, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vsyslog(priority, format, ap);
	va_end(ap);
}

void vsyslog(int priority, const char *message, va_list ap) {
	int cs;
	if(!(log_mask & LOG_MASK(priority & 7)) || (priority & ~0x3ff)) {
		mlibc::infoLogger() << "\e[31mmlibc: syslog: log_mask or priority out of range, not printing anything\e[39m" << frg::endlog;
		return;
	}
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	frg::unique_lock<FutexLock> lock(__syslog_lock);
	_vsyslog(priority, message, ap);
	pthread_setcancelstate(cs, 0);
}


#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <bits/ensure.h>

static char log_ident[32];
static int log_options;
static int log_facility = LOG_USER;
static int log_fd = -1;

static const struct sockaddr_un log_addr {AF_UNIX, "/dev/log"};

void closelog(void) {
	close(log_fd);
	log_fd = -1;
}

static void __openlog() {
	log_fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if(log_fd >= 0)
		connect(log_fd, (const sockaddr *)&log_addr, sizeof log_addr);
}

void openlog(const char *ident, int options, int facility) {
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

int setlogmask(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void syslog(int, const char *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void vsyslog(int, const char *, va_list) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


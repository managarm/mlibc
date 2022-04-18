#include <utmp.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

/*
 * The code in this file is largely based on glibc.
 * This includes:
 * - setutent
 * - read_last_entry
 * - getutent
 * - getutent_r
 * - endutent
 */
static int fd = -1;
static off_t offset;

static struct utmp last_entry;

void setutent(void) {
	if(fd < 0) {
		fd = open("/run/utmp", O_RDONLY | O_LARGEFILE | O_CLOEXEC);
		if(fd == -1) {
			return;
		}
	}

	lseek(fd, 0, SEEK_SET);
	offset = 0;
}

struct utmp *getutent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endutent(void) {
	if(fd >= 0) {
		close(fd);
		fd = -1;
	}
}

struct utmp *pututline(const struct utmp *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct utmp *getutline(const struct utmp *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

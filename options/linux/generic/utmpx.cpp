#include <bits/ensure.h>
#include <stddef.h>
#include <errno.h>
#include <utmpx.h>

void updwtmpx(const char *, const struct utmpx *) {
	// Empty as musl does
}

void endutxent(void) {
	// Empty as musl does
}

void setutxent(void) {
	// Empty as musl does
}

struct utmpx *getutxent(void) {
	// return NULL as musl does
	return NULL;
}

struct utmpx *pututxline(const struct utmpx *) {
	// return NULL as musl does
	return NULL;
}

int utmpxname(const char *) {
	// return -1 as musl does
	errno = ENOSYS;
	return -1;
}

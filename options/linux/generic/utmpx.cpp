#include <bits/ensure.h>
#include <stddef.h>
#include <errno.h>
#include <utmpx.h>
#include <mlibc/debug.hpp>

void updwtmpx(const char *, const struct utmpx *) {
	// Empty as musl does
	mlibc::infoLogger() << "\e[31mmlibc: updwtmpx() is a stub\e[39m" << frg::endlog;
}

void endutxent(void) {
	// Empty as musl does
	mlibc::infoLogger() << "\e[31mmlibc: endutxent() is a stub\e[39m" << frg::endlog;
}

void setutxent(void) {
	// Empty as musl does
	mlibc::infoLogger() << "\e[31mmlibc: setutxent() is a stub\e[39m" << frg::endlog;
}

struct utmpx *getutxent(void) {
	// return NULL as musl does
	mlibc::infoLogger() << "\e[31mmlibc: getutxent() is a stub\e[39m" << frg::endlog;
	return NULL;
}

struct utmpx *pututxline(const struct utmpx *) {
	// return NULL as musl does
	mlibc::infoLogger() << "\e[31mmlibc: pututxline() is a stub\e[39m" << frg::endlog;
	return NULL;
}

int utmpxname(const char *) {
	// return -1 as musl does
	mlibc::infoLogger() << "\e[31mmlibc: utmpxname() is a stub\e[39m" << frg::endlog;
	errno = ENOSYS;
	return -1;
}

struct utmpx *getutxid(const struct utmpx *) {
	// return NULL as musl does
	mlibc::infoLogger() << "\e[31mmlibc: getutxid() is a stub\e[39m" << frg::endlog;
	return NULL;
}

#include <bits/ensure.h>
#include <stddef.h>
#include <errno.h>
#include <utmpx.h>
#include <stdio.h>
#include <time.h>
#include <paths.h>
#include <unistd.h>
#include <fcntl.h>

int utmpx_file = -1;

void updwtmpx(const char *, const struct utmpx *) {
	// Empty as musl does
}

void endutxent(void) {
	if (utmpx_file >= 0) {
		close(utmpx_file);
	}
}

void setutxent(void) {
	if (utmpx_file < 0) {
		utmpx_file = open(UTMPX_FILE, O_RDWR | O_CREAT, 0664);
		if (utmpx_file < 0) {
			utmpx_file = open(UTMPX_FILE, O_RDONLY);
		}
	} else {
		lseek(utmpx_file, 0, SEEK_SET);
	}
}

struct utmpx returned;

struct utmpx *getutxent(void) {
	if (utmpx_file < 0) {
		setutxent();
		if (utmpx_file < 0) {
			return NULL;
		}
	}

	if (read(utmpx_file, &returned, sizeof(struct utmpx)) != sizeof(struct utmpx)) {
		return NULL;
	}

	return &returned;
}

struct utmpx *pututxline(const struct utmpx *added) {
	if (utmpx_file < 0) {
		setutxent();
		if (utmpx_file < 0) {
			return NULL;
		}
	}

	lseek(utmpx_file, 0, SEEK_END);
	if (write(utmpx_file, added, sizeof(struct utmpx)) != sizeof(struct utmpx)) {
		return NULL;
	}

	return (struct utmpx *)added;
}

int utmpxname(const char *path) {
	(void)path;
	endutxent();
	setutxent();
	if (utmpx_file > 0) {
		lseek(utmpx_file, 0, SEEK_END);
		return 1;
	} else {
		return 0;
	}
}

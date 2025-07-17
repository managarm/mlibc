#include <bits/ensure.h>
#include <errno.h>
#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>
#include <mlibc/debug.hpp>
#include <paths.h>
#include <stddef.h>
#include <stdlib.h>
#include <utmpx.h>

#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/utmp.hpp>

namespace {

constexpr const char *defaultUtmpxPath = UTMPX_FILE;

const char *utmpxPath = defaultUtmpxPath;
frg::ticket_spinlock utmpxMutex;

frg::optional<int> utmpxFd = frg::null_opt;

utmpx returned;

} // namespace

void updwtmpx(const char *file, const struct utmpx *ut) {
	int fd;
	int err = mlibc::sys_open(file, O_RDWR | O_CREAT | O_CLOEXEC | O_APPEND, 0644, &fd);
	if(err) {
		mlibc::infoLogger() << "\e[31mmlibc: updwtmpx() failed to open " << file << ": "
							<< strerror(err) << "\e[39m" << frg::endlog;
		return;
	}

	mlibc::putUtmpEntry(fd, ut);

	mlibc::sys_close(fd);
}

void endutxent(void) {
	frg::unique_lock lock{utmpxMutex};

	if(utmpxFd) {
		mlibc::sys_close(utmpxFd.value());
		utmpxFd = frg::null_opt;
	}
}

void setutxent(void) {
	frg::unique_lock lock{utmpxMutex};

	if(!utmpxFd) {
		int fd;
		int err = mlibc::sys_open(utmpxPath, O_RDWR | O_CREAT | O_CLOEXEC, 0644, &fd);
		if(err) {
			mlibc::infoLogger() << "\e[31mmlibc: setutxent() failed to open " << utmpxPath << ": "
			                    << strerror(err) << "\e[39m" << frg::endlog;
			utmpxFd = frg::null_opt;
		} else {
			utmpxFd = fd;
		}
	} else {
		off_t discard;
		mlibc::sys_seek(utmpxFd.value(), 0, SEEK_SET, &discard);
	}
}

struct utmpx *getutxent(void) {
	frg::unique_lock lock{utmpxMutex};

	if(!utmpxFd)
		setutxent();
	if(!utmpxFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::getUtmpEntry(*utmpxFd, &returned); e) {
		errno = e;
		return nullptr;
	}

	return &returned;
}

struct utmpx *pututxline(const struct utmpx *ut) {
	frg::unique_lock lock{utmpxMutex};

	if(!utmpxFd)
		setutxent();
	if(!utmpxFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::putUtmpEntry(*utmpxFd, ut); e) {
		errno = e;
		return nullptr;
	}

	return (utmpx *) ut;
}

int utmpxname(const char *file) {
	frg::unique_lock lock{utmpxMutex};

	if(strcmp(file, utmpxPath)) {
		if(!strcmp(file, defaultUtmpxPath)) {
			free((void *) utmpxPath);
			utmpxPath = defaultUtmpxPath;
		} else {
			char *name = strdup(file);
			if(!name)
				return -1;

			if(utmpxPath != defaultUtmpxPath)
				free((void *) utmpxPath);

			utmpxPath = name;
		}
	}

	return 0;
}

struct utmpx *getutxid(const struct utmpx *ut) {
	frg::unique_lock lock{utmpxMutex};

	if(!utmpxFd)
		setutxent();
	if(!utmpxFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::getUtmpEntryById(*utmpxFd, ut, &returned); e) {
		errno = e;
		return nullptr;
	}

	return &returned;
}

struct utmpx *getutxline(const struct utmpx *ut) {
	frg::unique_lock lock{utmpxMutex};

	if(!utmpxFd)
		setutxent();
	if(!utmpxFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::getUtmpEntryByType(*utmpxFd, ut, &returned); e) {
		errno = e;
		return nullptr;
	}

	return &returned;
}

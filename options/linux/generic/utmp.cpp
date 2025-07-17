#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>

#include <bits/ensure.h>
#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/utmp.hpp>
#include <mlibc/posix-sysdeps.hpp>

namespace {

constexpr const char *defaultUtmpPath = UTMP_FILE;

const char *utmpPath = defaultUtmpPath;
frg::ticket_spinlock utmpMutex;

frg::optional<int> utmpFd = frg::null_opt;

utmp returned;

} // namespace

void setutent(void) {
	frg::unique_lock lock{utmpMutex};

	if(!utmpFd) {
		int fd;
		int err = mlibc::sys_open(utmpPath, O_RDWR | O_CREAT | O_CLOEXEC, 0644, &fd);
		if(err) {
			mlibc::infoLogger() << "\e[31mmlibc: setutent() failed to open " << utmpPath << ": "
			                    << strerror(err) << "\e[39m" << frg::endlog;
			utmpFd = frg::null_opt;
		} else {
			utmpFd = fd;
		}
	} else {
		off_t discard;
		mlibc::sys_seek(utmpFd.value(), 0, SEEK_SET, &discard);
	}
}

struct utmp *getutent(void) {
	frg::unique_lock lock{utmpMutex};

	if(!utmpFd)
		setutent();
	if(!utmpFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::getUtmpEntry(*utmpFd, &returned); e) {
		errno = e;
		return nullptr;
	}

	return &returned;
}

int getutent_r(struct utmp *buf, struct utmp **res) {
	frg::unique_lock lock{utmpMutex};

	if(!utmpFd)
		setutent();
	if(!utmpFd) {
		*res = nullptr;
		errno = ENOENT;
		return -1;
	}

	if(int e = mlibc::getUtmpEntry(*utmpFd, buf); e) {
		*res = nullptr;
		errno = e;
		return -1;
	}

	*res = buf;
	return 0;
}

void endutent(void) {
	frg::unique_lock lock{utmpMutex};

	if(utmpFd) {
		mlibc::sys_close(utmpFd.value());
		utmpFd = frg::null_opt;
	}
}

struct utmp *pututline(const struct utmp *ut) {
	frg::unique_lock lock{utmpMutex};

	if(!utmpFd)
		setutent();
	if(!utmpFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::putUtmpEntry(*utmpFd, ut); e) {
		errno = e;
		return nullptr;
	}

	return (utmp *) ut;
}

struct utmp *getutline(const struct utmp *ut) {
	frg::unique_lock lock{utmpMutex};

	if(!utmpFd)
		setutent();
	if(!utmpFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::getUtmpEntryByType(*utmpFd, ut, &returned); e) {
		errno = e;
		return nullptr;
	}

	return &returned;
}

int utmpname(const char *file) {
	frg::unique_lock lock{utmpMutex};

	if(strcmp(file, utmpPath)) {
		if(!strcmp(file, defaultUtmpPath)) {
			free((void *) utmpPath);
			utmpPath = defaultUtmpPath;
		} else {
			char *name = strdup(file);
			if(!name)
				return -1;

			if(utmpPath != defaultUtmpPath)
				free((void *) utmpPath);

			utmpPath = name;
		}
	}

	return 0;
}

struct utmp *getutid(const struct utmp *ut) {
	frg::unique_lock lock{utmpMutex};

	if(!utmpFd)
		setutent();
	if(!utmpFd) {
		errno = ENOENT;
		return nullptr;
	}

	if(int e = mlibc::getUtmpEntryById(*utmpFd, ut, &returned); e) {
		errno = e;
		return nullptr;
	}

	return &returned;
}

void updwtmp(const char *file, const struct utmp *ut) {
	int fd;
	int err = mlibc::sys_open(file, O_RDWR | O_CREAT | O_CLOEXEC | O_APPEND, 0644, &fd);
	if(err) {
		mlibc::infoLogger() << "\e[31mmlibc: updwtmp() failed to open " << file << ": "
							<< strerror(err) << "\e[39m" << frg::endlog;
		return;
	}

	mlibc::putUtmpEntry(fd, ut);
	mlibc::sys_close(fd);
}

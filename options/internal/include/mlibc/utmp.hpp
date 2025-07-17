#pragma once

#include <abi-bits/errno.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/seek-whence.h>
#include <abi-bits/utmp-defines.h>
#include <bits/ensure.h>
#include <bits/posix/timeval.h>
#include <bits/size_t.h>
#include <mlibc/ansi-sysdeps.hpp>
#include <string.h>
#include <type_traits>

namespace mlibc {

template <class S>
concept UtmpStruct = requires(S s) {
	std::is_same_v<decltype(s.ut_user), char[]>;
	std::is_same_v<decltype(s.ut_id), char[]>;
	std::is_same_v<decltype(s.ut_line), char[]>;
	std::is_same_v<decltype(s.ut_pid), pid_t>;
	std::is_same_v<decltype(s.ut_type), short int>;
	std::is_same_v<decltype(s.ut_tv), struct timeval>;
};

template <UtmpStruct U>
int getUtmpEntry(int fd, U *res) {
	ssize_t progress = 0;
	ssize_t read = 0;
	char *ptr = reinterpret_cast<char *>(res);

	int err = mlibc::sys_read(fd, ptr, sizeof(U), &read);
	if(err)
		return err;

	if(read == sizeof(U))
		return 0;
	else if(read == 0)
		return ESRCH;

	progress = read;

	while(read) {
		err = mlibc::sys_read(fd, ptr + progress, sizeof(U) - progress, &read);
		if(err)
			return err;

		progress += read;

		if(progress == sizeof(U))
			return 0;
	}

	return ESRCH;
}

template <UtmpStruct U>
int getUtmpEntryById(int fd, const U *id, U *res) {
	while(true) {
		U tmp;
		if(int e = getUtmpEntry(fd, &tmp); e)
			return e;

		if(id->ut_type == RUN_LVL || id->ut_type == BOOT_TIME
		|| id->ut_type == NEW_TIME || id->ut_type == OLD_TIME) {
			if(tmp.ut_type == id->ut_type) {
				memcpy(res, &tmp, sizeof(U));
				return 0;
			}
		} else if(id->ut_type == INIT_PROCESS || id->ut_type == LOGIN_PROCESS
		|| id->ut_type == USER_PROCESS || id->ut_type == DEAD_PROCESS) {
			if(!memcmp(tmp.ut_id, id->ut_id, sizeof(U::ut_id))) {
				memcpy(res, &tmp, sizeof(U));
				return 0;
			}
		}
	}
}

template <UtmpStruct U>
int getUtmpEntryByType(int fd, const U *id, U *res) {
	while(true) {
		U tmp;
		if(int e = getUtmpEntry(fd, &tmp); e)
			return e;

		if(id->ut_type == USER_PROCESS || id->ut_type == LOGIN_PROCESS) {
			if(!strncmp(tmp.ut_line, id->ut_line, sizeof(U::ut_line))) {
				memcpy(res, &tmp, sizeof(U));
				return 0;
			}
		}
	}
}

template <UtmpStruct U>
int putUtmpEntry(int fd, const U *ut) {
	size_t progress = 0;
	char *ptr = (char *) ut;

	off_t discard;
	if(int e = mlibc::sys_seek(fd, 0, SEEK_END, &discard); e)
		return e;

	while(progress < sizeof(U)) {
		ssize_t written = 0;
		if(int e = mlibc::sys_write(fd, ptr + progress, sizeof(U) - progress, &written); e)
			return e;
		progress += written;
	}

	return 0;
}

} // namespace mlibc

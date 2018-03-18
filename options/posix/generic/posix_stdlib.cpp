
#include <bits/abi.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

long random(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// Path handling.
// ----------------------------------------------------------------------------

int mkstemp(char *pattern) {
	auto n = strlen(pattern);
	__ensure(n >= 6);
	if(n < 6) {
		errno = EINVAL;
		return -1;
	}
	for(size_t i = 0; i < 6; i++) {
		if(pattern[n - 6 + i] == 'X')
			continue;
		errno = EINVAL;
		return -1;
	}
	
	// TODO: Do an exponential search.
	for(size_t i = 0; i < 999999; i++) {
		__ensure(sprintf(pattern + (n - 6), "%06zu", i) == 6);
//		frigg::infoLogger() << "mlibc: mkstemp candidate is "
//				<< (const char *)pattern << frigg::endLog;

		// TODO: Add a mode argument to sys_open().
		int fd;
		if(!mlibc::sys_open(pattern, O_RDWR | O_CREAT | O_EXCL, /*S_IRUSR | S_IWUSR,*/ &fd))
			return fd;
		if(errno != EEXIST)
			return -1;
	}

	__ensure(errno == EEXIST);
	return -1;
}

char *mkdtemp(char *path) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *realpath(const char *__restrict path, char *__restrict resolved) {
	// TODO: Implement this based on 
	__ensure(!resolved);
	frigg::infoLogger() << "\e[31mmlibc: realpath() does not really resolve paths\e[39m"
			<< frigg::endLog;

	resolved = reinterpret_cast<char *>(malloc(strlen(path) + 1));
	assert(resolved);
	strcpy(resolved, path);
	return resolved;
}


#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/stat.h>
#include <bits/ensure.h>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/stdlib.hpp>
#include <stdio.h>
#include <string.h>

namespace mlibc {

int mkostemps(char *pattern, int suffixlen, int flags, int *fd) {
	auto n = strlen(pattern);
	if(n < (6 + static_cast<size_t>(suffixlen))) {
		return EINVAL;
	}

	flags &= ~O_WRONLY;

	for(size_t i = 0; i < 6; i++) {
		if(pattern[n - (6 + suffixlen) + i] == 'X')
			continue;
		return EINVAL;
	}

	// TODO: Do an exponential search.
	for(size_t i = 0; i < 999999; i++) {
		char sfx = pattern[n - suffixlen];
		__ensure(sprintf(pattern + (n - (6 + suffixlen)), "%06zu", i) == 6);
		pattern[n - suffixlen] = sfx;

		if(int e = mlibc::sys_open(pattern, O_RDWR | O_CREAT | O_EXCL | flags, S_IRUSR | S_IWUSR, fd); !e) {
			return 0;
		}else if(e != EEXIST) {
			return e;
		}
	}

	return EEXIST;
}

} // namespace mlibc

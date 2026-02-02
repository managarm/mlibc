/*
Copyright © 2026, __robot@PLT
SPDX-Licence-Identifier: MIT OR CC0
*/

#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/bsd-sysdeps.hpp>
#include <stdlib.h>
#include <sys/statvfs.h>

// Known bug from NetBSD implementation: A concurrency-safe implementation would allocate on each
// call so the application may call `free`. However, to implement NetBSD semantics, the allocation
// is stored here.
static struct statvfs *mounts_buffer = nullptr;

int getmntinfo(struct statvfs **mntbufp, int mode) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getvfsstat, -1);

	// Check how much capacity is needed.
	int cap = 0;
	sysdep(nullptr, 0, mode, &cap);

	int actual = cap;
	do {
		cap = actual;
		free(mounts_buffer);
		mounts_buffer = (struct statvfs *)calloc(cap, sizeof(struct statvfs));

		if (int e = sysdep(mounts_buffer, cap, mode, &actual); e) {
			mounts_buffer = nullptr;
			errno = e;
			return -1;
		}

		// Retry in a loop on the off chance a new FS just mounted,
		// though we can only care so much about concurrency due to NetBSD bugs.
	} while (cap < actual);

	*mntbufp = mounts_buffer;

	return actual;
}

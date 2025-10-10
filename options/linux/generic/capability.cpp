#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

#ifdef __cplusplus
extern "C" {
#endif

int capget(cap_user_header_t hdrp, cap_user_data_t datap) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_capget, -1);
	if(int e = mlibc::sys_capget(hdrp, datap); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int capset(cap_user_header_t hdrp, const cap_user_data_t datap) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_capset, -1);
	if(int e = mlibc::sys_capset(hdrp, datap); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

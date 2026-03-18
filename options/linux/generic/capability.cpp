#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

#ifdef __cplusplus
extern "C" {
#endif

int capget(cap_user_header_t hdrp, cap_user_data_t datap) {
	if(int e = mlibc::sysdep_or_enosys<Capget>(hdrp, datap); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int capset(cap_user_header_t hdrp, const cap_user_data_t datap) {
	if(int e = mlibc::sysdep_or_enosys<Capset>(hdrp, datap); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

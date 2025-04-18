#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>
#include <errno.h>
#include <assert.h>

int main(void) {
#if !defined(__GLIBC__) || (__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 32)
	const char *s = strerrordesc_np(EINVAL);
	assert(!strcmp(s, "Invalid argument (EINVAL)"));
	assert(strerrordesc_np(0) == NULL);
#endif
}

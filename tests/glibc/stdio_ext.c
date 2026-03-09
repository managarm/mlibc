#include <assert.h>
#include <stdio.h>
#include <stdio_ext.h>

int main() {
	FILE *f = fopen("/dev/zero", "r");
	assert(f != NULL);

	setvbuf(f, NULL, _IOLBF, 0);
	assert(__flbf(f) != 0);

	fgetc(f);
	assert(__fbufsize(f) > 0);

	setvbuf(f, NULL, _IOFBF, 0);
	assert(__flbf(f) == 0);

#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	// glibc does not support __fseterr
	assert(ferror(f) == 0);
	__fseterr(f);
	assert(ferror(f) != 0);
#endif

	fclose(f);
	return 0;
}

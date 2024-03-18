#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <string.h>

int main() {
	char test[19] = "Hello mlibc World!";
	char *alloca_ed = strdupa(test);
	assert(!strcmp(test, alloca_ed));

	char *trimmed = strndupa(test, 5);
	assert(!strcmp("Hello", trimmed));

	return 0;
}

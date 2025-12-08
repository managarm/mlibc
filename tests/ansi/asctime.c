#include <assert.h>
#include <string.h>
#include <time.h>

int main(void) {
	struct tm tm = {
		.tm_year = 70,
		.tm_mon = 0,
		.tm_mday = 1,
		.tm_wday = 4,
		.tm_hour = 0,
		.tm_min = 0,
		.tm_sec = 0,
	};
	char *result = asctime(&tm);
	assert(result);
	assert(!strcmp("Thu Jan  1 00:00:00 1970\n", result));
	return 0;
}

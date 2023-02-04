#include <string.h>
#include <time.h>
#include <assert.h>

int main() {
	char timebuf[16];
	char result[16] = " 8";
	struct tm tm;
	tm.tm_sec = 0;
	tm.tm_min = 17;
	tm.tm_hour = 17;
	tm.tm_mday = 8;
	tm.tm_mon = 2;
	tm.tm_year = 121;
	tm.tm_wday = 2;
	tm.tm_yday = 39;
	strftime(timebuf, sizeof(timebuf), "%e", &tm);
	assert(!strcmp(timebuf, result));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%x", &tm);
	assert(!strcmp(timebuf, "03/08/21"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%X", &tm);
	assert(!strcmp(timebuf, "17:17:00"));

	return 0;
}

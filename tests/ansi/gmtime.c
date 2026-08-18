#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

// int64_t instead of time_t so this compiles for 32-bit time_t systems.
static void check(int64_t time, int year, int mon, int mday, int hour, int min, int sec,
		int yday, int wday) {
	time_t t = (time_t) time;
	struct tm tm;
	memset(&tm, 0, sizeof(tm));
	assert(gmtime_r(&t, &tm) == &tm);
	printf("%lld -> %04d-%02d-%02d %02d:%02d:%02d yday=%d wday=%d\n", (long long)t,
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_yday, tm.tm_wday);
	assert(tm.tm_year == year - 1900);
	assert(tm.tm_mon == mon - 1);
	assert(tm.tm_mday == mday);
	assert(tm.tm_hour == hour);
	assert(tm.tm_min == min);
	assert(tm.tm_sec == sec);
	assert(tm.tm_yday == yday);
	assert(tm.tm_wday == wday);

	// The result has to convert back to the value we started from.
	assert(timegm(&tm) == t);
}

int main() {
	check(0, 1970, 1, 1, 0, 0, 0, 0, 4);
	check(1, 1970, 1, 1, 0, 0, 1, 0, 4);

	// Times before the epoch: the time of day must stay in range and the day
	// has to be carried back, rather than leaving negative fields behind.
	check(-1, 1969, 12, 31, 23, 59, 59, 364, 3);
	check(-86399, 1969, 12, 31, 0, 0, 1, 364, 3);
	check(-86400, 1969, 12, 31, 0, 0, 0, 364, 3);
	check(-86401, 1969, 12, 30, 23, 59, 59, 363, 2);
	check(-1000000000, 1938, 4, 24, 22, 13, 20, 113, 0);

	// tm_yday must account for leap years rather than assuming every year has
	// a 29th of February.
	check(1000000000, 2001, 9, 9, 1, 46, 40, 251, 0);
	check(951782400, 2000, 2, 29, 0, 0, 0, 59, 2);
	check(951868800, 2000, 3, 1, 0, 0, 0, 60, 3);
	check(68169600, 1972, 2, 29, 0, 0, 0, 59, 2);

	// 1900 is divisible by four but is not a leap year, so it has no 29th of
	// February and every day after it shifts by one.
	if (sizeof(time_t) > 4) {
		check(-2208988800, 1900, 1, 1, 0, 0, 0, 0, 1);
		check(-2203977600, 1900, 2, 28, 0, 0, 0, 58, 3);
		check(-2203891200, 1900, 3, 1, 0, 0, 0, 59, 4);
	}

	return 0;
}

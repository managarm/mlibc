#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdlib.h>
#include <time.h>

static void check_localtime(time_t time, int year, int mon, int mday, int hour, int min,
		int sec, int isdst) {
	struct tm tm;
	assert(localtime_r(&time, &tm) == &tm);
	assert(tm.tm_year == year - 1900);
	assert(tm.tm_mon == mon - 1);
	assert(tm.tm_mday == mday);
	assert(tm.tm_hour == hour);
	assert(tm.tm_min == min);
	assert(tm.tm_sec == sec);
	assert(tm.tm_isdst == isdst);
}

int main() {
	// POSIX TZ: US Eastern time, changing at 02:00 local time on the second
	// Sunday in March and the first Sunday in November.
	assert(setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0/2", 1) == 0);
	tzset();
	check_localtime(1741503599, 2025, 3, 9, 1, 59, 59, 0);
	check_localtime(1741503600, 2025, 3, 9, 3, 0, 0, 1);

	// POSIX TZ: UTC standard and DST offsets with Julian-day transitions.
	assert(setenv("TZ", "STD0DST0,J60/0,J300/0", 1) == 0);
	tzset();
	check_localtime(1740787200, 2025, 3, 1, 0, 0, 0, 1);

	// POSIX TZ: UTC standard and DST offsets, starting on February's last Sunday.
	assert(setenv("TZ", "STD0DST0,M2.5.0/0,M3.1.0/0", 1) == 0);
	tzset();
	check_localtime(1740355200, 2025, 2, 24, 0, 0, 0, 1);

	return 0;
}

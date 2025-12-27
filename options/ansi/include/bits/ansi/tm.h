#ifndef _MLIBC_ANSI_TM
#define _MLIBC_ANSI_TM

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	long int tm_gmtoff;
	const char *tm_zone;
};

#endif /* _MLIBC_ANSI_TM */


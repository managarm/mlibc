#ifndef _LINUX_RTC_H
#define _LINUX_RTC_H

#include <sys/ioctl.h>

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

#define RTC_RD_TIME	_IOR('p', 0x09, struct rtc_time)
#define RTC_SET_TIME	_IOW('p', 0x0a, struct rtc_time)

#endif // _LINUX_RTC_H

#ifndef _SYS_IRONCLAD_DEVICES_H
#define _SYS_IRONCLAD_DEVICES_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <asm/ioctls.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PS2MOUSE_2_1_SCALING     1
#define PS2MOUSE_1_1_SCALING     2
#define PS2MOUSE_SET_RES         3
#define PS2MOUSE_SET_SAMPLE_RATE 4

struct ironclad_mouse_data {
	int  x_variation;
	int  y_variation;
	bool is_left;
	bool is_right;
};

#define RTC_RD_TIME  1
#define RTC_SET_TIME 2

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;     /* unused */
	int tm_yday;     /* unused */
	int tm_isdst;    /* unused */
};

#ifdef __cplusplus
}
#endif

#endif // _SYS_IRONCLAD_DEVICES_H

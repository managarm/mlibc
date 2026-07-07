#ifndef _PIGGY_HWCLOCK_H
#define _PIGGY_HWCLOCK_H

struct rtc_time {
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
};

#endif /* _PIGGY_HWCLOCK_H */

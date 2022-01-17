
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <wchar.h>
#include <stdlib.h>
#include <ctype.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/file-window.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/lock.hpp>
#include <mlibc/bitutil.hpp>

#include <frg/mutex.hpp>

const char __utc[] = "UTC";

// Variables defined by POSIX.
int daylight;
long timezone;
char *tzname[2];

static FutexLock __time_lock;
static file_window *get_localtime_window() {
	static file_window window{"/etc/localtime"};
	return &window;
}

// Function taken from musl
clock_t clock(void) {
	struct timespec ts;

	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts))
		return -1;

	if(ts.tv_sec > LONG_MAX / 1000000 || ts.tv_nsec / 1000 > LONG_MAX - 1000000 * ts.tv_sec)
		return -1;

	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

double difftime(time_t a, time_t b) {
	return a - b;
}

time_t mktime(struct tm *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

/* There is no other implemented value than TIME_UTC; all other values
 * are considered erroneous. */
// Function taken from musl
int timespec_get(struct timespec *ts, int base) {
	if(base != TIME_UTC)
		return 0;
	int ret = clock_gettime(CLOCK_REALTIME, ts);
	return ret < 0 ? 0 : base;
}

char *asctime(const struct tm *ptr) {
	static char buf[26];
	return asctime_r(ptr, buf);
}

char *ctime(const time_t *timer) {
	struct tm *tm = localtime(timer);
	if(!tm) {
		return 0;
	}
	return asctime(tm);
}

struct tm *gmtime(const time_t *unix_gmt) {
	static thread_local struct tm per_thread_tm;
	return gmtime_r(unix_gmt, &per_thread_tm);
}

struct tm *localtime(const time_t *unix_gmt) {
	tzset();
	static thread_local struct tm per_thread_tm;
	return localtime_r(unix_gmt, &per_thread_tm);
}

size_t strftime(char *__restrict dest, size_t max_size,
		const char *__restrict format, const struct tm *__restrict tm) {
	auto c = format;
	auto p = dest;

	while(*c) {
		auto space = (dest + max_size) - p;
		__ensure(space >= 0);

		if(*c != '%') {
			if(!space)
				return 0;
			*p = *c;
			c++;
			p++;
			continue;
		}

		if(*(c + 1) == 'Y') {
			auto chunk = snprintf(p, space, "%d", 1900 + tm->tm_year);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'm') {
			auto chunk = snprintf(p, space, "%.2d", tm->tm_mon + 1);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'd') {
			auto chunk = snprintf(p, space, "%.2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'Z') {
			auto chunk = snprintf(p, space, "%s", "GMT");
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'H') {
			auto chunk = snprintf(p, space, "%.2i", tm->tm_hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'M') {
			auto chunk = snprintf(p, space, "%.2i", tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'S') {
			auto chunk = snprintf(p, space, "%.2d", tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'R') {
			auto chunk = snprintf(p, space, "%.2i:%.2i", tm->tm_hour, tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'T') {
			auto chunk = snprintf(p, space, "%.2i:%.2i:%.2i", tm->tm_hour, tm->tm_min, tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'F') {
			auto chunk = snprintf(p, space, "%d/%.2d/%.2d", 1900 + tm->tm_year, tm->tm_mon + 1,
					tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'a') {
			const char *strdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			int day = tm->tm_wday;
			if(day < 0 || day > 6)
				__ensure(!"Day not in bounds.");

			auto chunk = snprintf(p, space, "%s", strdays[day]);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'b' || *(c + 1) == 'B') {
			const char *strmons[12];
			switch (*(c + 1)) {
				case 'b':
					strmons[0] = "Jan";
					strmons[1] = "Feb";
					strmons[2] = "Mar";
					strmons[3] = "Apr";
					strmons[4] = "May";
					strmons[5] = "Jun";
					strmons[6] = "Jul";
					strmons[7] = "Aug";
					strmons[8] = "Sep";
					strmons[9] = "Oct";
					strmons[10] = "Nov";
					strmons[11] = "Dec";
					break;
				case 'B':
					strmons[0] = "January";
					strmons[1] = "February";
					strmons[2] = "March";
					strmons[3] = "April";
					strmons[4] = "May";
					strmons[5] = "June";
					strmons[6] = "July";
					strmons[7] = "August";
					strmons[8] = "September";
					strmons[9] = "October";
					strmons[10] = "November";
					strmons[11] = "December";
					break;
			}
			int mon = tm->tm_mon;
			if(mon < 0 || mon > 11)
				__ensure(!"Month not in bounds.");

			auto chunk = snprintf(p, space, "%s", strmons[mon]);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'c') {
			auto chunk = snprintf(p, space, "%d/%.2d/%.2d %.2d:%.2d:%.2d", 1900 + tm->tm_year,
					tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'd') {
			auto chunk = snprintf(p, space, "%.2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'e') {
			int chunk;
			chunk = snprintf(p, space, "%2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'I') {
			int hour = tm->tm_hour;
			if(hour > 12)
				hour -= 12;
			auto chunk = snprintf(p, space, "%.2i", hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'p') {
			if(tm->tm_hour < 12) {
				char time[] = "AM";
				auto chunk = snprintf(p, space, "%s", time);
				if(chunk >= space)
					return 0;
				p += chunk;
			}else {
				char time[] = "PM";
				auto chunk = snprintf(p, space, "%s", time);
				if(chunk >= space)
					return 0;
				p += chunk;
			}
			c += 2;
		}else {
			__ensure(!"Unknown format type.");
		}
	}

	auto space = (dest + max_size) - p;
	if(!space)
		return 0;

	*p = '\0';
	return (p - dest) + 1;
}

size_t wcsftime(wchar_t *__restrict, size_t, const wchar_t *__restrict,
		const struct tm *__restrict) MLIBC_STUB_BODY
namespace {

struct tzfile {
	uint8_t magic[4];
	uint8_t version;
	uint8_t reserved[15];
	uint32_t tzh_ttisgmtcnt;
	uint32_t tzh_ttisstdcnt;
	uint32_t tzh_leapcnt;
	uint32_t tzh_timecnt;
	uint32_t tzh_typecnt;
	uint32_t tzh_charcnt;
};

struct[[gnu::packed]] ttinfo {
	int32_t tt_gmtoff;
	unsigned char tt_isdst;
	unsigned char tt_abbrind;
};

}

// TODO(geert): this function doesn't parse the TZ environment variable
// or properly handle the case where information might be missing from /etc/localtime
// also we should probably unify the code for this and unix_local_from_gmt()
void tzset(void) {
	frg::unique_lock<FutexLock> lock(__time_lock);
	// TODO(geert): we can probably cache this somehow
	tzfile tzfile_time;
	memcpy(&tzfile_time, reinterpret_cast<char *>(get_localtime_window()->get()), sizeof(tzfile));
	tzfile_time.tzh_ttisgmtcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisgmtcnt);
	tzfile_time.tzh_ttisstdcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisstdcnt);
	tzfile_time.tzh_leapcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_leapcnt);
	tzfile_time.tzh_timecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_timecnt);
	tzfile_time.tzh_typecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_typecnt);
	tzfile_time.tzh_charcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_charcnt);

	if(tzfile_time.magic[0] != 'T' || tzfile_time.magic[1] != 'Z' || tzfile_time.magic[2] != 'i'
			|| tzfile_time.magic[3] != 'f') {
		mlibc::infoLogger() << "mlibc: /etc/localtime is not a valid TZinfo file" << frg::endlog;
		return;
	}

	if(tzfile_time.version != '\0' && tzfile_time.version != '2' && tzfile_time.version != '3') {
		mlibc::infoLogger() << "mlibc: /etc/localtime has an invalid TZinfo version"
				<< frg::endlog;
		return;
	}

	// There should be at least one entry in the ttinfo table.
	// TODO: If there is not, we might want to fall back to UTC, no DST (?).
	__ensure(tzfile_time.tzh_typecnt);

	char *abbrevs = reinterpret_cast<char *>(get_localtime_window()->get()) + sizeof(tzfile)
		+ tzfile_time.tzh_timecnt * sizeof(int32_t)
		+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
		+ tzfile_time.tzh_typecnt * sizeof(struct ttinfo);
	// start from the last ttinfo entry, this matches the behaviour of glibc and musl
	for (int i = tzfile_time.tzh_typecnt; i > 0; i--) {
		ttinfo time_info;
		memcpy(&time_info, reinterpret_cast<char *>(get_localtime_window()->get()) + sizeof(tzfile)
				+ tzfile_time.tzh_timecnt * sizeof(int32_t)
				+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
				+ i * sizeof(ttinfo), sizeof(ttinfo));
		time_info.tt_gmtoff = mlibc::bit_util<uint32_t>::byteswap(time_info.tt_gmtoff);
		if (!time_info.tt_isdst && !tzname[0]) {
			tzname[0] = abbrevs + time_info.tt_abbrind;
			timezone = -time_info.tt_gmtoff;
		}
		if (time_info.tt_isdst && !tzname[1]) {
			tzname[1] = abbrevs + time_info.tt_abbrind;
			timezone = -time_info.tt_gmtoff;
			daylight = 1;
		}
	}
}

// POSIX extensions.

int nanosleep(const struct timespec *req, struct timespec *) {
	if (req->tv_sec < 0 || req->tv_nsec > 999999999 || req->tv_nsec < 0) {
		errno = EINVAL;
		return -1;
	}

	if(!mlibc::sys_sleep) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}

	struct timespec tmp = *req;

	int e = mlibc::sys_sleep(&tmp.tv_sec, &tmp.tv_nsec);
	if (!e) {
		return 0;
	} else {
		errno = e;
		return -1;
	}
}

int clock_getres(clockid_t clockid, struct timespec *res) {
	if(!mlibc::sys_clock_getres) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_clock_getres(clockid, &res->tv_sec, &res->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int clock_gettime(clockid_t clock, struct timespec *time) {
	if(int e = mlibc::sys_clock_get(clock, &time->tv_sec, &time->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int clock_nanosleep(clockid_t, int, const struct timespec *, struct timespec *) {
	__ensure(!"clock_nanosleep() not implemented");
	__builtin_unreachable();
}

int clock_settime(clockid_t, const struct timespec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

time_t time(time_t *out) {
	time_t secs;
	long nanos;
	if(int e = mlibc::sys_clock_get(CLOCK_REALTIME, &secs, &nanos); e) {
		errno = e;
		return (time_t)-1;
	}
	if(out)
		*out = secs;
	return secs;
}

namespace {

void civil_from_days(time_t days_since_epoch, int *year, unsigned int *month, unsigned int *day) {
	time_t time = days_since_epoch + 719468;
	int era = (time >= 0 ? time : time - 146096) / 146097;
	unsigned int doe = static_cast<unsigned int>(time - era * 146097);
	unsigned int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;
	int y = static_cast<int>(yoe) + era * 400;
	unsigned int doy = doe - (365*yoe + yoe/4 - yoe/100);
	unsigned int mp = (5*doy + 2)/153;
	unsigned int d = doy - (153*mp+2)/5 + 1;
	unsigned int m = mp + (mp < 10 ? 3 : -9);

	*year = y + (m <= 2);
	*month = m;
	*day = d;
}

void weekday_from_days(time_t days_since_epoch, unsigned int *weekday) {
	*weekday = static_cast<unsigned int>(days_since_epoch >= -4 ?
			(days_since_epoch+4) % 7 : (days_since_epoch+5) % 7 + 6);
}

void yearday_from_date(unsigned int year, unsigned int month, unsigned int day, unsigned int *yday) {
	unsigned int n1 = 275 * month / 9;
	unsigned int n2 = (month + 9) / 12;
	unsigned int n3 = (1 + (year - 4 * year / 4 + 2) / 3);
	*yday = n1 - (n2 * n3) + day - 30;
}

// Looks up the local time rules for a given
// UNIX GMT timestamp (seconds since 1970 GMT, ignoring leap seconds).
// This function assumes the __time_lock has been taken
// TODO(geert): if /etc/localtime isn't available this will fail... In that case
// we should call tzset() and use the variables to compute the variables from
// the tzset() global variables. Look at the musl code for how to do that
int unix_local_from_gmt(time_t unix_gmt, time_t *offset, bool *dst, char **tm_zone) {
	tzfile tzfile_time;
	memcpy(&tzfile_time, reinterpret_cast<char *>(get_localtime_window()->get()), sizeof(tzfile));
	tzfile_time.tzh_ttisgmtcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisgmtcnt);
	tzfile_time.tzh_ttisstdcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisstdcnt);
	tzfile_time.tzh_leapcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_leapcnt);
	tzfile_time.tzh_timecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_timecnt);
	tzfile_time.tzh_typecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_typecnt);
	tzfile_time.tzh_charcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_charcnt);

	if(tzfile_time.magic[0] != 'T' || tzfile_time.magic[1] != 'Z' || tzfile_time.magic[2] != 'i'
			|| tzfile_time.magic[3] != 'f') {
		mlibc::infoLogger() << "mlibc: /etc/localtime is not a valid TZinfo file" << frg::endlog;
		return -1;
	}

	if(tzfile_time.version != '\0' && tzfile_time.version != '2' && tzfile_time.version != '3') {
		mlibc::infoLogger() << "mlibc: /etc/localtime has an invalid TZinfo version"
				<< frg::endlog;
		return -1;
	}

	int index = -1;
	for(size_t i = 0; i < tzfile_time.tzh_timecnt; i++) {
		int32_t ttime;
		memcpy(&ttime, reinterpret_cast<char *>(get_localtime_window()->get()) + sizeof(tzfile)
				+ i * sizeof(int32_t), sizeof(int32_t));
		ttime = mlibc::bit_util<uint32_t>::byteswap(ttime);
		// If we are before the first transition, the format dicates that
		// the first ttinfo entry should be used (and not the ttinfo entry pointed
		// to by the first transition time).
		if(i && ttime > unix_gmt) {
			index = i - 1;
			break;
		}
	}

	// The format dictates that if no transition is applicable,
	// the first entry in the file is chosen.
	uint8_t ttinfo_index = 0;
	if(index >= 0) {
		memcpy(&ttinfo_index, reinterpret_cast<char *>(get_localtime_window()->get()) + sizeof(tzfile)
				+ tzfile_time.tzh_timecnt * sizeof(int32_t)
				+ index * sizeof(uint8_t), sizeof(uint8_t));
	}

	// There should be at least one entry in the ttinfo table.
	// TODO: If there is not, we might want to fall back to UTC, no DST (?).
	__ensure(tzfile_time.tzh_typecnt);

	ttinfo time_info;
	memcpy(&time_info, reinterpret_cast<char *>(get_localtime_window()->get()) + sizeof(tzfile)
			+ tzfile_time.tzh_timecnt * sizeof(int32_t)
			+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
			+ ttinfo_index * sizeof(ttinfo), sizeof(ttinfo));
	time_info.tt_gmtoff = mlibc::bit_util<uint32_t>::byteswap(time_info.tt_gmtoff);

	char *abbrevs = reinterpret_cast<char *>(get_localtime_window()->get()) + sizeof(tzfile)
		+ tzfile_time.tzh_timecnt * sizeof(int32_t)
		+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
		+ tzfile_time.tzh_typecnt * sizeof(struct ttinfo);

	*offset = time_info.tt_gmtoff;
	*dst = time_info.tt_isdst;
	*tm_zone = abbrevs + time_info.tt_abbrind;
	return 0;
}

} //anonymous namespace

struct tm *gmtime_r(const time_t *unix_gmt, struct tm *res) {
	int year;
	unsigned int month;
	unsigned int day;
	unsigned int weekday;
	unsigned int yday;

	time_t unix_local = *unix_gmt;

	int days_since_epoch = unix_local / (60*60*24);
	civil_from_days(days_since_epoch, &year, &month, &day);
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	res->tm_sec = unix_local % 60;
	res->tm_min = (unix_local / 60) % 60;
	res->tm_hour = (unix_local / (60*60)) % 24;
	res->tm_mday = day;
	res->tm_mon = month - 1;
	res->tm_year = year - 1900;
	res->tm_wday = weekday;
	res->tm_yday = yday - 1;
	res->tm_isdst = -1;
	res->tm_zone = __utc;
	res->tm_gmtoff = 0;

	return res;
}

struct tm *localtime_r(const time_t *unix_gmt, struct tm *res) {
	int year;
	unsigned int month;
	unsigned int day;
	unsigned int weekday;
	unsigned int yday;

	time_t offset = 0;
	bool dst;
	char *tm_zone;
	frg::unique_lock<FutexLock> lock(__time_lock);
	// TODO: Set errno if the conversion fails.
	if(unix_local_from_gmt(*unix_gmt, &offset, &dst, &tm_zone)) {
		__ensure(!"Error parsing /etc/localtime");
		__builtin_unreachable();
	}
	time_t unix_local = *unix_gmt + offset;

	int days_since_epoch = unix_local / (60*60*24);
	civil_from_days(days_since_epoch, &year, &month, &day);
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	res->tm_sec = unix_local % 60;
	res->tm_min = (unix_local / 60) % 60;
	res->tm_hour = (unix_local / (60*60)) % 24;
	res->tm_mday = day;
	res->tm_mon = month - 1;
	res->tm_year = year - 1900;
	res->tm_wday = weekday;
	res->tm_yday = yday - 1;
	res->tm_isdst = dst;
	res->tm_zone = tm_zone;
	res->tm_gmtoff = offset;

	return res;
}

// This implementation of asctime_r is taken from sortix
char *asctime_r(const struct tm *tm, char *buf) {
	static char weekday_names[7][4] =
		{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	static char month_names[12][4] =
		{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
		  "Nov", "Dec" };
	sprintf(buf, "%.3s %.3s%3d %.2d:%.2d%.2d %d\n",
				 weekday_names[tm->tm_wday],
				 month_names[tm->tm_mon],
				 tm->tm_mday,
				 tm->tm_hour,
				 tm->tm_min,
				 tm->tm_sec,
				 tm->tm_year + 1900);
	return buf;
}

char *ctime_r(const time_t *clock, char *buf) {
	return asctime_r(localtime(clock), buf);
}

namespace {

int month_to_day(int month) {
	switch(month){
		case  0: return 0;
		case  1: return 31;
		case  2: return 59;
		case  3: return 90;
		case  4: return 120;
		case  5: return 151;
		case  6: return 181;
		case  7: return 212;
		case  8: return 243;
		case  9: return 273;
		case 10: return 304;
		case 11: return 334;
	}
	return -1;
}

int is_leapyear(int year) {
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

int month_and_year_to_day_in_year(int month, int year){
	int day = month_to_day(month);
	if(is_leapyear(year) && month < 2)
		return day + 1;

	return day;
}

int target_determination(int month) {
	switch(month){
		case 0: return 3;
		case 1: return 14;
		case 2: return 14;
		case 3: return 4;
		case 4: return 9;
		case 5: return 6;
		case 6: return 11;
		case 7: return 8;
		case 8: return 5;
		case 9: return 10;
		case 10: return 7;
		case 11: return 12;
	}

	return -1;
}

int doom_determination(int full_year) {
	int century = full_year / 100;
	int anchor = 2 + 5 * (century % 4) % 7;

	int year = full_year % 100;

	if(year % 2)
		year += 11;

	year /= 2;

	if(year % 2)
		year += 11;

	return 7 - (year % 7) + anchor;
}

//Determine day of week through the doomsday algorithm.
int day_determination(int day, int month, int year) {
	int doom = doom_determination(year);
	bool leap = is_leapyear(year);

	int target = target_determination(month);
	if(leap && month < 2)
		target++;

	int doom_dif = (day - target) % 7;
	return (doom + doom_dif) % 7;
}

struct strptime_internal_state {
	bool has_century;
	bool has_year;
	bool has_month;
	bool has_day_of_month;
	bool has_day_of_year;
	bool has_day_of_week;

	bool full_year_given;

	int century;

	size_t format_index;
	size_t input_index;
};

char *strptime_internal(const char *__restrict input, const char *__restrict format,
	struct tm *__restrict tm, struct strptime_internal_state *__restrict state) {
	while(isspace(input[state->input_index]))
		state->input_index++;

	if(input[state->input_index] == '\0')
		return NULL;

	while(format[state->format_index] != '\0'){
		if(format[state->format_index] != '%'){
			if(isspace(format[state->format_index])){
				while(isspace(input[state->input_index++]));
				state->input_index--;
			}
			else {
				if(format[state->format_index] != input[state->input_index++])
					return NULL;
			}
			state->format_index++;
			continue;
		}
		state->format_index++;
		switch(format[state->format_index]){
			case '%':
				if(input[state->input_index++] != '%')
					return NULL;
				break;
			case 'a':
			case 'A':
				__ensure(!"strptime() %a and %A directives unimplemented.");
				__builtin_unreachable();
				break;
			case 'b':
			case 'B':
			case 'h':
				__ensure(!"strptime() %b, %B and %h directives unimplemented.");
				__builtin_unreachable();
				break;
			case 'c':
				__ensure(!"strptime() %c directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'C': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				state->input_index += n;
				state->century = product;
				state->has_century = true;
				break;
			}
			case 'd': //`%d` and `%e` are equivalent
			case 'e': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				state->input_index += n;
				tm->tm_mday = product;
				state->has_day_of_month = true;
				break;
			}
			case 'D': { //equivalent to `%m/%d/%y`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%m/%d/%y", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'H': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				state->input_index += n;
				tm->tm_hour = product;
				break;
			}
			case 'I':
				__ensure(!"strptime() %I directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'j': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 3 < n)
					return NULL;
				state->input_index += n;
				tm->tm_yday = product - 1;
				state->has_day_of_year = true;
				break;
			}
			case 'm': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				state->input_index += n;
				tm->tm_mon = product - 1;
				state->has_month = true;
				break;
			}
			case 'M': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				state->input_index += n;
				tm->tm_min = product;
				break;
			}
			case 'n':
			case 't': {
				size_t n = 0;
				while(isspace(input[state->input_index++]))
					n++;
				if(n == 0)
					return NULL;
				state->input_index--;
				break;
			}
			case 'p':
				__ensure(!"strptime() %p directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'r':
				__ensure(!"strptime() %r directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'R': { //equivalent to `%H:%M`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%H:%M", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'S': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				state->input_index += n;
				tm->tm_sec = product;
				break;
			}
			case 'T': { //equivalent to `%H:%M:%S`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%H:%M:%S", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'U':
				__ensure(!"strptime() %U directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'w': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 1 < n)
					return NULL;
				state->input_index += n;
				tm->tm_wday = product;
				state->has_day_of_week = true;
				break;
			}
			case 'W':
				__ensure(!"strptime() %W directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'x':
				__ensure(!"strptime() %x directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'X':
				__ensure(!"strptime() %X directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'y': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				if(product < 69)
					product += 100;
				state->input_index += n;
				tm->tm_year = product;
				state->has_year = true;
				break;
			}
			case 'Y': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 4 < n)
					return NULL;
				state->input_index += n;
				tm->tm_year = product - 1900;
				state->has_year = true;
				state->has_century = true;
				state->full_year_given = true;
				state->century = product / 100;
				break;
			}
			case 'F': { //GNU extensions
				//equivalent to `%Y-%m-%d`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%Y-%m-%d", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'g':
				__ensure(!"strptime() %g directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'G':
				__ensure(!"strptime() %G directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'u': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 1 < n)
					return NULL;
				state->input_index += n;
				tm->tm_wday = product - 1;
				break;
			}
			case 'V':
				__ensure(!"strptime() %V directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'z':
				__ensure(!"strptime() %z directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'Z':
				__ensure(!"strptime() %Z directive unimplemented.");
				__builtin_unreachable();
				break;
			case 's': //end of GNU extensions
				__ensure(!"strptime() %s directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'E': { //locale-dependent date & time representation
				__ensure(!"strptime() %E* directives unimplemented.");
				__builtin_unreachable();
				state->format_index++;
				switch(format[state->format_index]){
					case 'c':
						break;
					case 'C':
						break;
					case 'x':
						break;
					case 'X':
						break;
					case 'y':
						break;
					case 'Y':
						break;
					default:
						return NULL;
				}
			}
			case 'O': { //locale-dependent numeric symbols
				__ensure(!"strptime() %O* directives unimplemented.");
				__builtin_unreachable();
				state->format_index++;
				switch(format[state->format_index]){
					case 'd':
					case 'e':
						break;
					case 'H':
						break;
					case 'I':
						break;
					case 'm':
						break;
					case 'M':
						break;
					case 'S':
						break;
					case 'U':
						break;
					case 'w':
						break;
					case 'W':
						break;
					case 'y':
						break;
					default:
						return NULL;
				}
			}
			default:
				return NULL;
		}
		state->format_index++;
	}

	return (char*)input + state->input_index;
}

} //anonymous namespace

char *strptime(const char *__restrict s, const char *__restrict format, struct tm *__restrict tm){
	struct strptime_internal_state state = {};

	char *result = strptime_internal(s, format, tm, &state);

	if(result == NULL)
		return NULL;

	if(state.has_century && !state.full_year_given){
		int full_year = state.century * 100;

		if(state.has_year){
			//Compensate for default century-adjustment of `%j` operand
			if(tm->tm_year >= 100)
				full_year += tm->tm_year - 100;
			else
				full_year += tm->tm_year;
		}

		tm->tm_year = full_year - 1900;

		state.has_year = true;
	}

	if(state.has_month && !state.has_day_of_year){
		int day = 0;
		if(state.has_year)
			day = month_and_year_to_day_in_year(tm->tm_mon, tm->tm_year);
		else
			day = month_to_day(tm->tm_mon);

		tm->tm_yday = day + tm->tm_mday - 1;
		state.has_day_of_year = true;
	}

	if(state.has_year && !state.has_day_of_week){
		if(!state.has_month && !state.has_day_of_month){
			tm->tm_wday = day_determination(0, 0, tm->tm_year + 1900);
		}
		else if(state.has_month && state.has_day_of_month){
			tm->tm_wday = day_determination(tm->tm_mday, tm->tm_mon, tm->tm_year + 1900);
		}
		state.has_day_of_week = true;
	}

	return result;
}

time_t timelocal(struct tm *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

time_t timegm(struct tm *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

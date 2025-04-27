#include <algorithm>
#include <array>
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
#include <mlibc/locale.hpp>
#include <mlibc/bitutil.hpp>
#include <mlibc/strings.hpp>

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

time_t mktime(struct tm *tm) {
	return timegm(tm);
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
	[[maybe_unused]] bool use_alternative_symbols = false;
	[[maybe_unused]] bool use_alternative_era_format = false;

	while(*c) {
		int chunk;
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

		if(*(c + 1) == 'O') {
			std::array<char, 15> valid{{'B', 'b', 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'}};
			auto next = *(c + 2);
			if(std::find(valid.begin(), valid.end(), next) != valid.end()) {
				use_alternative_symbols = true;
				c++;
			} else {
				*p = '%';
				p++;
				c++;
				*p = 'O';
				p++;
				c++;
				continue;
			}
		} else if(*(c + 1) == 'E') {
			std::array<char, 6> valid{{'c', 'C', 'x', 'X', 'y', 'Y'}};
			auto next = *(c + 2);
			if(std::find(valid.begin(), valid.end(), next) != valid.end()) {
				use_alternative_era_format = true;
				c++;
			} else {
				*p = '%';
				p++;
				c++;
				*p = 'E';
				p++;
				c++;
				continue;
			}
		}

		switch(*++c) {
		case 'Y': {
			chunk = snprintf(p, space, "%d", 1900 + tm->tm_year);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'm': {
			chunk = snprintf(p, space, "%.2d", tm->tm_mon + 1);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'd': {
			chunk = snprintf(p, space, "%.2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'z': {
			auto min = tm->tm_gmtoff / 60;
			auto diff = ((min / 60) * 100) + (min % 60);
			chunk = snprintf(p, space, "%c%04d", diff >= 0 ? '+' : '-', abs(diff));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'Z': {
			chunk = snprintf(p, space, "%s", "UTC");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'H': {
			chunk = snprintf(p, space, "%.2i", tm->tm_hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'M': {
			chunk = snprintf(p, space, "%.2i", tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'S': {
			chunk = snprintf(p, space, "%.2d", tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'R': {
			chunk = snprintf(p, space, "%.2i:%.2i", tm->tm_hour, tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'T': {
			chunk = snprintf(p, space, "%.2i:%.2i:%.2i", tm->tm_hour, tm->tm_min, tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'F': {
			chunk = snprintf(p, space, "%d-%.2d-%.2d", 1900 + tm->tm_year, tm->tm_mon + 1,
					tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'D': {
			chunk = snprintf(p, space, "%.2d/%.2d/%.2d", tm->tm_mon + 1, tm->tm_mday, (tm->tm_year + 1900) % 100);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'a': {
			int day = tm->tm_wday;
			if(day < 0 || day > 6)
				__ensure(!"Day not in bounds.");

			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo(ABDAY_1 + day));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'b':
		case 'B':
		case 'h': {
			int mon = tm->tm_mon;
			if(mon < 0 || mon > 11)
				__ensure(!"Month not in bounds.");

			nl_item item = (*c == 'B') ? MON_1 : ABMON_1;

			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo(item + mon));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'c': {
			int day = tm->tm_wday;
			if(day < 0 || day > 6)
				__ensure(!"Day not in bounds.");

			int mon = tm->tm_mon;
			if(mon < 0 || mon > 11)
				__ensure(!"Month not in bounds.");

			chunk = snprintf(p, space, "%s %s %2d %.2i:%.2i:%.2d %d", mlibc::nl_langinfo(ABDAY_1 + day),
					mlibc::nl_langinfo(ABMON_1 + mon), tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, 1900 + tm->tm_year);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'e': {
			chunk = snprintf(p, space, "%2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'l': {
			int hour = tm->tm_hour;
			if(!hour)
				hour = 12;
			if(hour > 12)
				hour -= 12;
			chunk = snprintf(p, space, "%2d", hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'k': {
			chunk = snprintf(p, space, "%2d", tm->tm_hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'I': {
			int hour = tm->tm_hour;
			if(!hour)
				hour = 12;
			if(hour > 12)
				hour -= 12;
			chunk = snprintf(p, space, "%.2d", hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'p': {
			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo((tm->tm_hour < 12) ? AM_STR : PM_STR));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'P': {
			char *str = mlibc::nl_langinfo((tm->tm_hour < 12) ? AM_STR : PM_STR);
			char *str_lower = reinterpret_cast<char *>(getAllocator().allocate(strlen(str) + 1));
			for(size_t i = 0; str[i]; i++)
				str_lower[i] = tolower(str[i]);
			str_lower[strlen(str)] = '\0';

			chunk = snprintf(p, space, "%s", str_lower);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'C': {
			chunk = snprintf(p, space, "%.2d", (1900 + tm->tm_year) / 100);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'y': {
			chunk = snprintf(p, space, "%.2d", (1900 + tm->tm_year) % 100);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'j': {
			chunk = snprintf(p, space, "%.3d", tm->tm_yday + 1);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'A': {
			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo(DAY_1 + tm->tm_wday));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'r': {
			int hour = tm->tm_hour;
			if(!hour)
				hour = 12;
			if(hour > 12)
				hour -= 12;
			chunk = snprintf(p, space, "%.2i:%.2i:%.2i %s", hour, tm->tm_min, tm->tm_sec,
				mlibc::nl_langinfo((tm->tm_hour < 12) ? AM_STR : PM_STR));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case '%': {
			chunk = snprintf(p, space, "%%");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'n': {
			chunk = snprintf(p, space, "\n");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 't': {
			chunk = snprintf(p, space, "\t");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'x': {
			return strftime(dest, max_size, mlibc::nl_langinfo(D_FMT), tm);
		}
		case 'X': {
			return strftime(dest, max_size, mlibc::nl_langinfo(T_FMT), tm);
		}
		case '\0': {
			chunk = snprintf(p, space, "%%");
			if(chunk >= space)
				return 0;
			p += chunk;
			break;
		}
		default:
			mlibc::panicLogger() << "mlibc: strftime unknown format type: " << c << frg::endlog;
		}
	}

	auto space = (dest + max_size) - p;
	if(!space)
		return 0;

	*p = '\0';
	return (p - dest);
}

size_t wcsftime(wchar_t *__restrict, size_t, const wchar_t *__restrict,
		const struct tm *__restrict) {
	mlibc::infoLogger() << "mlibc: wcsftime is a stub" << frg::endlog;
	return 0;
}

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
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clock_getres, -1);
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

int clock_nanosleep(clockid_t clockid, int, const struct timespec *req, struct timespec *) {
	mlibc::infoLogger() << "clock_nanosleep is implemented as nanosleep!" << frg::endlog;
	__ensure(clockid == CLOCK_REALTIME || clockid == CLOCK_MONOTONIC);
	return nanosleep(req, nullptr);
}

int clock_settime(clockid_t clock, const struct timespec *time) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clock_set, -1);
	if(int e = mlibc::sys_clock_set(clock, time->tv_sec, time->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
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

time_t timelocal(struct tm *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

constexpr static int days_from_civil(int y, unsigned m, unsigned d) noexcept {
	y -= m <= 2;
	const int era = (y >= 0 ? y : y - 399) / 400;
	const unsigned yoe = static_cast<unsigned>(y - era * 400); // [0, 399]
	const unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1; // [0, 365]
	const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy; // [0, 146096]
	return era * 146097 + static_cast<int>(doe) - 719468;
}

time_t timegm(struct tm *tm) {
	time_t year = tm->tm_year + 1900;
	time_t month = tm->tm_mon + 1;
	time_t days = days_from_civil(year, month, tm->tm_mday);
	time_t secs = (days * 86400) + (tm->tm_hour * 60 * 60) + (tm->tm_min * 60) + tm->tm_sec;
	return secs;
}

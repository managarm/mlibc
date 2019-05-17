
#include <errno.h>
#include <byteswap.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <wchar.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/file-window.hpp>
#include <mlibc/sysdeps.hpp>

clock_t clock(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
double difftime(time_t a, time_t b){
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
time_t mktime(struct tm *ptr){
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int timespec_get(struct timespec *ptr, int base){
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *asctime(const struct tm *ptr){
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *ctime(const time_t *timer){
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct tm *gmtime(const time_t *unix_gmt) {
	static thread_local struct tm per_thread_tm;
	return gmtime_r(unix_gmt, &per_thread_tm);
}

struct tm *localtime(const time_t *unix_gmt) {
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
		}else if (*(c + 1) == 'b') {
			const char *strmons[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
					"Aug", "Sep", "Oct", "Nov", "Dec" };
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

void tzset(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// POSIX extensions.

int nanosleep(const struct timespec *, struct timespec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clock_getres(clockid_t, struct timespec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

int utimes(const char *, const struct timeval[2]) {
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

// Looks up the local time rules for a given 
// UNIX GMT timestamp (seconds since 1970 GMT, ignoring leap seconds).
int unix_local_from_gmt(time_t unix_gmt, time_t *offset, bool *dst) {
	static file_window window{"/etc/localtime"};

	tzfile tzfile_time;
	memcpy(&tzfile_time, reinterpret_cast<char *>(window.get()), sizeof(tzfile));
	tzfile_time.tzh_ttisgmtcnt = bswap_32(tzfile_time.tzh_ttisgmtcnt);
	tzfile_time.tzh_ttisstdcnt = bswap_32(tzfile_time.tzh_ttisstdcnt);
	tzfile_time.tzh_leapcnt = bswap_32(tzfile_time.tzh_leapcnt);
	tzfile_time.tzh_timecnt = bswap_32(tzfile_time.tzh_timecnt);
	tzfile_time.tzh_typecnt = bswap_32(tzfile_time.tzh_typecnt);
	tzfile_time.tzh_charcnt = bswap_32(tzfile_time.tzh_charcnt);
	
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
		memcpy(&ttime, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
				+ i * sizeof(int32_t), sizeof(int32_t));
		ttime = bswap_32(ttime);
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
		memcpy(&ttinfo_index, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
				+ tzfile_time.tzh_timecnt * sizeof(int32_t)
				+ index * sizeof(uint8_t), sizeof(uint8_t));
	}

	// There should be at least one entry in the ttinfo table.
	// TODO: If there is not, we might want to fall back to UTC, no DST (?).
	__ensure(tzfile_time.tzh_typecnt);

	ttinfo time_info;
	memcpy(&time_info, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
			+ tzfile_time.tzh_timecnt * sizeof(int32_t)
			+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
			+ ttinfo_index * sizeof(ttinfo), sizeof(ttinfo));
	time_info.tt_gmtoff = bswap_32(time_info.tt_gmtoff);
	
	*offset = time_info.tt_gmtoff;
	*dst = time_info.tt_isdst;
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
	// TODO: Set errno if the conversion fails.
	if(unix_local_from_gmt(*unix_gmt, &offset, &dst))
		__ensure(!"Error parsing /etc/localtime");
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
	res->tm_isdst = -1;

	return res;
}


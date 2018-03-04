
#include <time.h>

#include <string.h>
#include <bits/ensure.h>
#include <frigg/debug.hpp>
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
struct tm *gmtime(const time_t *timer) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct tm *localtime(const time_t *t) {
	static thread_local struct tm per_thread_tm;
	return localtime_r(t, &per_thread_tm);
}
size_t strftime(char *__restrict dest, size_t max_size,
		const char *__restrict format, const struct tm *__restrict tm) {
	auto c = format;
	auto p = dest;
	
	while(*c) {
		auto space = (dest + max_size) - p;
		assert(space >= 0);
		
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
			auto chunk = snprintf(p, space, "%d", tm->tm_mon + 1);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'd') {
			auto chunk = snprintf(p, space, "%d", tm->tm_mday);
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
			auto chunk = snprintf(p, space, "%i", tm->tm_hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'M') {
			auto chunk = snprintf(p, space, "%i", tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'S') {
			auto chunk = snprintf(p, space, "%d", tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'F') {
			auto chunk = snprintf(p, space, "%d/%d/%d", 1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'a') {
			const char *strdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			int day = tm->tm_wday;
			if(day < 0 || day > 6)
				assert(!"Day not in bounds.");

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
				assert(!"Month not in bounds.");
			
			auto chunk = snprintf(p, space, "%s", strmons[mon]);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'c') {
			auto chunk = snprintf(p, space, "%d/%d/%d %d:%d:%d", 1900 + tm->tm_year,
					tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'd') {
			auto chunk = snprintf(p, space, "%d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c += 2;
		}else if (*(c + 1) == 'I') {
			int hour = tm->tm_hour;
			if(hour > 12)
				hour -= 12;
			auto chunk = snprintf(p, space, "%i", hour);
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
			assert(!"Unknown format type.");
		}
	}

	auto space = (dest + max_size) - p;
	if(!space)
		return 0;	

	*p = '\0';
	return (p - dest) + 1;
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
	if(mlibc::sys_clock_get(clock, &time->tv_sec, &time->tv_nsec))
		return -1;
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
	long nanos;
	if(mlibc::sys_clock_get(CLOCK_REALTIME, out, &nanos))
		return (time_t)-1;
	return 0;
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

} //anonymous namespace

struct tm *localtime_r(const time_t *t, struct tm *tm) {
	int year;
	unsigned int month;
	unsigned int day;
	unsigned int weekday;
	unsigned int yday;

	int days_since_epoch = *timep / (60*60*24);
	civil_from_days(days_since_epoch, &year, &month, &day);
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	tm->tm_sec = *timep % 60;
	tm->tm_min = (*timep / 60) % 60;
	tm->tm_hour = (*timep / (60*60)) % 24;
	tm->tm_mday = day;
	tm->tm_mon = month - 1;
	tm->tm_year = year - 1900;
	tm->tm_wday = weekday;
	tm->tm_yday = yday - 1;
	tm->tm_isdst = -1;

	return tm;
}


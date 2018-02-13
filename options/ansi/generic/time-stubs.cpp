
#include <time.h>

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
		const char *__restrict, const struct tm *__restrict) {
	frigg::infoLogger() << "\e[31mmlibc: strftime always writes an"
		" empty string\e[39m" << frigg::endLog;
	__ensure(max_size > 0);
	dest[0] = 0;
	return 0;
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

int clock_gettime(clockid_t, struct timespec *time) {
	frigg::infoLogger() << "\e[31mmlibc: clock_gettime does not support different clocks"
		"\e[39m" << frigg::endLog;
	if(mlibc::sys_clock_get(&time->tv_sec))
		return -1;
	time->tv_nsec = 0;
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

struct tm *localtime_r(const time_t *t, struct tm *tm) {
	frigg::infoLogger() << "\e[31mmlibc: localtime_r always returns a"
		" zero date\e[39m" << frigg::endLog;
	memset(tm, 0, sizeof(struct tm));
	return tm;
}

time_t time(time_t *out) {
	if(mlibc::sys_clock_get(out))
		return (time_t)-1;
	return 0;
}


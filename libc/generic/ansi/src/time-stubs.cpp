
#include <time.h>

#include <mlibc/ensure.h>

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
// time() is provided by the platform
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
struct tm *localtime(const time_t *timer) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t strftime(char *__restrict dest, size_t max_size,
		const char *__restrict format, const struct tm *__restrict ptr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}



#include <inttypes.h>
#include <mlibc/ensure.h>

intmax_t imaxabs(intmax_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
imaxdiv_t imaxdiv(intmax_t, intmax_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
intmax_t strtoimax(const char *__restrict, char **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uintmax_t strtoumax(const char *__restrict, char **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
intmax_t wcstoimax(const wchar_t *__restrict, wchar_t **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uintmax_t wcstoumax(const wchar_t *__restrict, wchar_t **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

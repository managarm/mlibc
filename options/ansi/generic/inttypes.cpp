#include <bits/ensure.h>
#include <inttypes.h>
#include <mlibc/debug.hpp>
#include <mlibc/strtol.hpp>

intmax_t imaxabs(intmax_t num) {
	return num < 0 ? -num : num;
}

imaxdiv_t imaxdiv(intmax_t number, intmax_t denom) {
	imaxdiv_t r;
	r.quot = number / denom;
	r.rem = number % denom;
	return r;
}

intmax_t strtoimax(const char *it, char **out, int base) {
	// TODO: This function has to check for overflow!
	return mlibc::stringToInteger<intmax_t, char>(it, out, base);
}

uintmax_t strtoumax(const char *it, char **out, int base) {
	return mlibc::stringToInteger<uintmax_t, char>(it, out, base);
}

intmax_t wcstoimax(const wchar_t *__restrict it, wchar_t **__restrict out, int base) {
	// TODO: This function has to check for overflow!
	return mlibc::stringToInteger<intmax_t, wchar_t>(it, out, base);
}

uintmax_t wcstoumax(const wchar_t *__restrict it, wchar_t **__restrict out, int base) {
	return mlibc::stringToInteger<uintmax_t, wchar_t>(it, out, base);
}

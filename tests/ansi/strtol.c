#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <wchar.h>

#define DO_TEST(str, value, off, func, base) ({ \
	char s[] = (str); \
	char *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == 0); \
	assert(pEnd == (off == -1 ? s + strlen(s) : s + off)); })

#define DO_ERR_TEST(str, value, err, func, base) ({ \
	char s[] = (str); \
	char *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == err); \
	assert(pEnd == (err == ERANGE ? s + strlen(s) : s)); })

#define DO_TESTL(str, value, off, func, base) ({ \
	wchar_t s[] = (str); \
	wchar_t *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == 0); \
	assert(pEnd == (off == -1 ? s + wcslen(s) : s + off)); })

#define DO_ERR_TESTL(str, value, err, func, base) ({ \
	wchar_t s[] = (str); \
	wchar_t *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == err); \
	assert(pEnd == (err == ERANGE ? s + wcslen(s) : s)); })

int main () {
	// A few generic checks.
	DO_TEST("0", 0, -1, strtol, 0);
	DO_TEST("0", 0, -1, strtol, 10);
	DO_TEST("2001", 2001, -1, strtol, 10);
	DO_TEST("+2001", 2001, -1, strtol, 10);
	DO_TEST("60c0c0", 0x60c0c0, -1, strtol, 16);
	DO_TEST("-1101110100110100100000", -3624224, -1, strtol, 2);
	DO_TEST("0x6fffff", 0x6fffff, -1, strtol, 0);
	DO_TEST("0666", 0666, -1, strtol, 0);
#ifndef USE_HOST_LIBC
	DO_TEST("1100", 0b1100, -1, strtol, 2);
	DO_TEST("0b1100", 0b1100, -1, strtol, 0);
	DO_TEST("0B1100", 0b1100, -1, strtol, 0);
	DO_TEST("0b1zzz", 1, 3, strtol, 0);
#endif
	DO_TEST("0xzzz", 0, 1, strtol, 0);
	DO_TEST("0yzzz", 0, 1, strtol, 0);
	DO_TEST("00xzz", 0, 2, strtol, 0);
	DO_ERR_TEST("", 0, 0, strtol, 10);
	DO_ERR_TEST("asd", 0, 0, strtol, 10);
	DO_ERR_TEST("999999999999999999999999999", LONG_MAX, ERANGE, strtol, 10);
	DO_ERR_TEST("-999999999999999999999999999", LONG_MIN, ERANGE, strtol, 10);

	// strtol
#if defined(__i386__) || defined(__m68k__)
 	DO_TEST("-2147483648", LONG_MIN, -1, strtol, 10);
	DO_TEST("2147483647", LONG_MAX, -1, strtol, 10);
 	DO_ERR_TEST("-2147483649", LONG_MIN, ERANGE, strtol, 10);
	DO_ERR_TEST("2147483648", LONG_MAX, ERANGE, strtol, 10);
#else
	DO_TEST("-9223372036854775808", LONG_MIN, -1, strtol, 10);
	DO_TEST("9223372036854775807", LONG_MAX, -1, strtol, 10);
	DO_ERR_TEST("9223372036854775808", LONG_MAX, ERANGE, strtol, 10);
	DO_ERR_TEST("-9223372036854775809", LONG_MIN, ERANGE, strtol, 10);
#endif

	// wcstol
#if defined(__i386__) || defined(__m68k__)
	DO_TESTL(L"-2147483648", LONG_MIN, -1, wcstol, 10);
	DO_TESTL(L"2147483647", LONG_MAX, -1, wcstol, 10);
	DO_ERR_TESTL(L"2147483648", LONG_MAX, ERANGE, wcstol, 10);
	DO_ERR_TESTL(L"-2147483649", LONG_MIN, ERANGE, wcstol, 10);
#else
	DO_TESTL(L"-9223372036854775808", LONG_MIN, -1, wcstol, 10);
	DO_TESTL(L"9223372036854775807", LONG_MAX, -1, wcstol, 10);
	DO_ERR_TESTL(L"9223372036854775808", LONG_MAX, ERANGE, wcstol, 10);
	DO_ERR_TESTL(L"-9223372036854775809", LONG_MIN, ERANGE, wcstol, 10);
#endif

	// strtoll
	DO_TEST("-9223372036854775808", LLONG_MIN, -1, strtoll, 10);
	DO_TEST("9223372036854775807", LLONG_MAX, -1, strtoll, 10);
	DO_ERR_TEST("9223372036854775808", LLONG_MAX, ERANGE, strtoll, 10);
	DO_ERR_TEST("-9223372036854775809", LLONG_MIN, ERANGE, strtoll, 10);
	// wcstoll
	DO_TESTL(L"-9223372036854775808", LLONG_MIN, -1, wcstoll, 10);
	DO_TESTL(L"9223372036854775807", LLONG_MAX, -1, wcstoll, 10);
	DO_ERR_TESTL(L"9223372036854775808", LLONG_MAX, ERANGE, wcstoll, 10);
	DO_ERR_TESTL(L"-9223372036854775809", LLONG_MIN, ERANGE, wcstoll, 10);

	// strtoul
#if defined(__i386__) || defined(__m68k__)
	DO_TEST("-1", -(1UL), -1, strtoul, 10);
	DO_TEST("2147483647", LONG_MAX, -1, strtoul, 10);
	DO_TEST("4294967295", ULONG_MAX, -1, strtoul, 10);
	DO_TEST("-4294967295", 1UL, -1, strtoul, 10);
	DO_ERR_TEST("4294967296", ULONG_MAX, ERANGE, strtoul, 10);
#else
	DO_TEST("-1", -(1UL), -1, strtoul, 10);
	DO_TEST("9223372036854775807", LONG_MAX, -1, strtoul, 10);
	DO_TEST("18446744073709551615", ULONG_MAX, -1, strtoul, 10);
	DO_TEST("-18446744073709551615", 1UL, -1, strtoul, 10);
	DO_ERR_TEST("18446744073709551616", ULONG_MAX, ERANGE, strtoul, 10);
#endif

	// wcstoul
#if defined(__i386__) || defined(__m68k__)
	DO_TESTL(L"-1", -(1UL), -1, wcstoul, 10);
	DO_TESTL(L"2147483647", LONG_MAX, -1, wcstoul, 10);
	DO_TESTL(L"4294967295", ULONG_MAX, -1, wcstoul, 10);
	DO_TESTL(L"-4294967295", 1UL, -1, wcstoul, 10);
	DO_ERR_TESTL(L"4294967296", ULONG_MAX, ERANGE, wcstoul, 10);
#else
	DO_TESTL(L"-1", -(1UL), -1, wcstoul, 10);
	DO_TESTL(L"9223372036854775807", LONG_MAX, -1, wcstoul, 10);
	DO_TESTL(L"18446744073709551615", ULONG_MAX, -1, wcstoul, 10);
	DO_TESTL(L"-18446744073709551615", 1UL, -1, wcstoul, 10);
	DO_ERR_TESTL(L"18446744073709551616", ULONG_MAX, ERANGE, wcstoul, 10);
#endif

	// strtoull
	DO_TEST("-1", -(1ULL), -1, strtoull, 10);
	DO_TEST("9223372036854775807", LLONG_MAX, -1, strtoull, 10);
	DO_TEST("18446744073709551615", ULLONG_MAX, -1, strtoull, 10);
	DO_TEST("-18446744073709551615", 1ULL, -1, strtoull, 10);
	DO_ERR_TEST("18446744073709551616", ULLONG_MAX, ERANGE, strtoull, 10);
	// wcstoull
	DO_TESTL(L"-1", -(1ULL), -1, wcstoull, 10);
	DO_TESTL(L"9223372036854775807", LLONG_MAX, -1, wcstoull, 10);
	DO_TESTL(L"18446744073709551615", ULLONG_MAX, -1, wcstoull, 10);
	DO_TESTL(L"-18446744073709551615", 1ULL, -1, wcstoull, 10);
	DO_ERR_TESTL(L"18446744073709551616", ULLONG_MAX, ERANGE, wcstoull, 10);

	return 0;
}

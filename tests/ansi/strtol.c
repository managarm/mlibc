#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <wchar.h>

#define DO_TEST(str, value, func, base) ({ \
	char s[] = (str); \
	char *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == 0); \
	assert(pEnd == s + strlen(s)); })

#define DO_ERR_TEST(str, value, err, func, base) ({ \
	char s[] = (str); \
	char *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == err); \
	assert(pEnd == (err == ERANGE ? s + strlen(s) : s)); })

#define DO_TESTL(str, value, func, base) ({ \
	wchar_t s[] = (str); \
	wchar_t *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == 0); \
	assert(pEnd == s + wcslen(s)); })

#define DO_ERR_TESTL(str, value, err, func, base) ({ \
	wchar_t s[] = (str); \
	wchar_t *pEnd = NULL; \
	errno = 0; \
	assert(func(s, &pEnd, base) == (value)); \
	assert(errno == err); \
	assert(pEnd == (err == ERANGE ? s + wcslen(s) : s)); })

int main () {
	// A few generic checks.
	DO_TEST("0", 0, strtol, 0);
	DO_TEST("0", 0, strtol, 10);
	DO_TEST("2001", 2001, strtol, 10);
	DO_TEST("+2001", 2001, strtol, 10);
	DO_TEST("60c0c0", 0x60c0c0, strtol, 16);
	DO_TEST("-1101110100110100100000", -3624224, strtol, 2);
	DO_TEST("0x6fffff", 0x6fffff, strtol, 0);
	DO_TEST("0666", 0666, strtol, 0);
	DO_ERR_TEST("", 0, 0, strtol, 10);
	DO_ERR_TEST("asd", 0, 0, strtol, 10);
	DO_ERR_TEST("0xzzz", 0, 0, strtol, 0);
	DO_ERR_TEST("999999999999999999999999999", LONG_MAX, ERANGE, strtol, 10);
	DO_ERR_TEST("-999999999999999999999999999", LONG_MIN, ERANGE, strtol, 10);

	// strtol
	DO_TEST("-9223372036854775808", LONG_MIN, strtol, 10);
	DO_TEST("9223372036854775807", LONG_MAX, strtol, 10);
	DO_ERR_TEST("9223372036854775808", LONG_MAX, ERANGE, strtol, 10);
	DO_ERR_TEST("-9223372036854775809", LONG_MIN, ERANGE, strtol, 10);
	// wcstol
	DO_TESTL(L"-9223372036854775808", LONG_MIN, wcstol, 10);
	DO_TESTL(L"9223372036854775807", LONG_MAX, wcstol, 10);
	DO_ERR_TESTL(L"9223372036854775808", LONG_MAX, ERANGE, wcstol, 10);
	DO_ERR_TESTL(L"-9223372036854775809", LONG_MIN, ERANGE, wcstol, 10);

	// strtoll
	DO_TEST("-9223372036854775808", LONG_MIN, strtoll, 10);
	DO_TEST("9223372036854775807", LONG_MAX, strtoll, 10);
	DO_ERR_TEST("9223372036854775808", LLONG_MAX, ERANGE, strtol, 10);
	DO_ERR_TEST("-9223372036854775809", LLONG_MIN, ERANGE, strtol, 10);
	// wcstoll
	DO_TESTL(L"-9223372036854775808", LONG_MIN, wcstoll, 10);
	DO_TESTL(L"9223372036854775807", LONG_MAX, wcstoll, 10);
	DO_ERR_TESTL(L"9223372036854775808", LLONG_MAX, ERANGE, wcstol, 10);
	DO_ERR_TESTL(L"-9223372036854775809", LLONG_MIN, ERANGE, wcstol, 10);

	// strtoul
	DO_TEST("-1", -(1UL), strtoul, 10);
	DO_TEST("9223372036854775807", LONG_MAX, strtoul, 10);
	DO_TEST("18446744073709551615", ULONG_MAX, strtoul, 10);
	DO_TEST("-18446744073709551615", 1UL, strtoul, 10);
	DO_ERR_TEST("18446744073709551616", ULONG_MAX, ERANGE, strtoul, 10);
	// wcstoul
	DO_TESTL(L"-1", -(1UL), wcstoul, 10);
	DO_TESTL(L"9223372036854775807", LONG_MAX, wcstoul, 10);
	DO_TESTL(L"18446744073709551615", ULONG_MAX, wcstoul, 10);
	DO_TESTL(L"-18446744073709551615", 1UL, wcstoul, 10);
	DO_ERR_TESTL(L"18446744073709551616", ULONG_MAX, ERANGE, wcstoul, 10);

	// strtoull
	DO_TEST("-1", -(1UL), strtoull, 10);
	DO_TEST("9223372036854775807", LONG_MAX, strtoull, 10);
	DO_TEST("18446744073709551615", ULONG_MAX, strtoull, 10);
	DO_TEST("-18446744073709551615", 1UL, strtoull, 10);
	DO_ERR_TEST("18446744073709551616", ULONG_MAX, ERANGE, strtoull, 10);
	// wcstoull
	DO_TESTL(L"-1", -(1UL), wcstoull, 10);
	DO_TESTL(L"9223372036854775807", LONG_MAX, wcstoull, 10);
	DO_TESTL(L"18446744073709551615", ULONG_MAX, wcstoull, 10);
	DO_TESTL(L"-18446744073709551615", 1UL, wcstoull, 10);
	DO_ERR_TESTL(L"18446744073709551616", ULONG_MAX, ERANGE, wcstoull, 10);

	return 0;
}

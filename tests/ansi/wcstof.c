#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <wchar.h>

#define FLT_RANGE 0.000001f
#define DBL_RANGE 0.000001
#define LDBL_RANGE 0.000001

#define DO_TEST(str, value, off, func, range) ({ \
	wchar_t s[] = (str); \
	wchar_t *pEnd = NULL; \
	__typeof(func(s, &pEnd)) result = func(s, &pEnd); \
	assert(result >= (value) - (range)); \
	assert(result <= (value) + (range)); \
	assert(pEnd == (off == -1 ? s + wcslen(s) : s + off)); })

#define DO_TEST_SUCCESS_FUNC(str, success_func, off, func) ({ \
	wchar_t s[] = (str); \
	wchar_t *pEnd = NULL; \
	assert(success_func(func(s, &pEnd))); \
	assert(pEnd == (off == -1 ? s + wcslen(s) : s + off)); })

int main () {
	setlocale(LC_ALL, "C");

	DO_TEST(L"0", 0.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0.12", 0.12f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"12", 12.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"12.13", 12.13f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"10.0e1", 100.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"10.0e10", 100000000000.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"100.0e-1", 10.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x0", 0.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x0.12", 0.0703125f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x12", 18.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x12.13", 18.07421875f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x10.0p1", 32.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x10.0p10", 16384.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"0x100.0p-1", 128.0f, -1, wcstof, FLT_RANGE);
	DO_TEST(L"42.1end", 42.1f, 4, wcstof, FLT_RANGE);
	DO_TEST(L"42.1pinvalid", 42.1f, 4, wcstof, FLT_RANGE);
	DO_TEST(L"42.1nope", 42.1f, 4, wcstof, FLT_RANGE);
	DO_TEST_SUCCESS_FUNC(L"NAN", isnan, -1, wcstof);
	DO_TEST_SUCCESS_FUNC(L"nan", isnan, -1, wcstof);
	DO_TEST_SUCCESS_FUNC(L"INF", isinf, -1, wcstof);
	DO_TEST_SUCCESS_FUNC(L"INFINITY", isinf, -1, wcstof);

	DO_TEST(L"0", 0.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0.12", 0.12, -1, wcstod, DBL_RANGE);
	DO_TEST(L"12", 12.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"12.13", 12.13, -1, wcstod, DBL_RANGE);
	DO_TEST(L"10.0e1", 100.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"10.0e10", 100000000000.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"100.0e-1", 10.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x0", 0.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x0.12", 0.0703125, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x12", 18.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x12.13", 18.07421875, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x10.0p1", 32.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x10.0p10", 16384.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"0x100.0p-1", 128.0, -1, wcstod, DBL_RANGE);
	DO_TEST(L"42.1end", 42.1, 4, wcstod, DBL_RANGE);
	DO_TEST(L"42.1pinvalid", 42.1, 4, wcstod, DBL_RANGE);
	DO_TEST(L"42.1nope", 42.1, 4, wcstod, DBL_RANGE);
	DO_TEST_SUCCESS_FUNC(L"NAN", isnan, -1, wcstod);
	DO_TEST_SUCCESS_FUNC(L"nan", isnan, -1, wcstod);
	DO_TEST_SUCCESS_FUNC(L"INF", isinf, -1, wcstod);
	DO_TEST_SUCCESS_FUNC(L"INFINITY", isinf, -1, wcstod);

	DO_TEST(L"0", 0.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0.12", 0.12, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"12", 12.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"12.13", 12.13, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"10.0e1", 100.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"10.0e10", 100000000000.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"100.0e-1", 10.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x0", 0.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x0.12", 0.0703125, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x12", 18.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x12.13", 18.07421875, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x10.0p1", 32.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x10.0p10", 16384.0, -1, wcstold, LDBL_RANGE);
	DO_TEST(L"0x100.0p-1", 128.0, -1, wcstold, LDBL_RANGE);\
	DO_TEST(L"42.1end", 42.1, 4, wcstold, LDBL_RANGE);
	DO_TEST(L"42.1pinvalid", 42.1, 4, wcstold, LDBL_RANGE);
	DO_TEST(L"42.1nope", 42.1, 4, wcstold, LDBL_RANGE);
	DO_TEST_SUCCESS_FUNC(L"NAN", isnan, -1, wcstold);
	DO_TEST_SUCCESS_FUNC(L"nan", isnan, -1, wcstold);
	DO_TEST_SUCCESS_FUNC(L"INF", isinf, -1, wcstold);
	DO_TEST_SUCCESS_FUNC(L"INFINITY", isinf, -1, wcstold);

	return 0;
}

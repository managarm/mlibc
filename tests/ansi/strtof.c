#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define FLT_RANGE 0.000001f
#define DBL_RANGE 0.000001
#define LDBL_RANGE 0.000001

#define DO_TEST(str, value, off, func, range) ({ \
	char s[] = (str); \
	char *pEnd = NULL; \
	__typeof(func(s, &pEnd)) result = func(s, &pEnd); \
	assert(result >= (value) - (range)); \
	assert(result <= (value) + (range)); \
	assert(pEnd == (off == -1 ? s + strlen(s) : s + off)); })

#define DO_TEST_SUCCESS_FUNC(str, success_func, off, func) ({ \
	char s[] = (str); \
	char *pEnd = NULL; \
	assert(success_func(func(s, &pEnd))); \
	assert(pEnd == (off == -1 ? s + strlen(s) : s + off)); })

int main () {
	DO_TEST("0", 0.0f, -1, strtof, FLT_RANGE);
	DO_TEST("0.12", 0.12f, -1, strtof, FLT_RANGE);
	DO_TEST("12", 12.0f, -1, strtof, FLT_RANGE);
	DO_TEST("12.13", 12.13f, -1, strtof, FLT_RANGE);
	DO_TEST("10.0e1", 100.0f, -1, strtof, FLT_RANGE);
	DO_TEST("10.0e10", 100000000000.0f, -1, strtof, FLT_RANGE);
	DO_TEST("100.0e-1", 10.0f, -1, strtof, FLT_RANGE);
	DO_TEST("0x0", 0.0f, -1, strtof, FLT_RANGE);
	DO_TEST("0x0.12", 0.0703125f, -1, strtof, FLT_RANGE);
	DO_TEST("0x12", 18.0f, -1, strtof, FLT_RANGE);
	DO_TEST("0x12.13", 18.07421875f, -1, strtof, FLT_RANGE);
	DO_TEST("0x10.0p1", 32.0f, -1, strtof, FLT_RANGE);
	DO_TEST("0x10.0p10", 16384.0f, -1, strtof, FLT_RANGE);
	DO_TEST("0x100.0p-1", 128.0f, -1, strtof, FLT_RANGE);
	DO_TEST_SUCCESS_FUNC("NAN", isnan, -1, strtof);
	DO_TEST_SUCCESS_FUNC("nan", isnan, -1, strtof);
	DO_TEST_SUCCESS_FUNC("INF", isinf, -1, strtof);
	DO_TEST_SUCCESS_FUNC("INFINITY", isinf, -1, strtof);

	DO_TEST("0", 0.0, -1, strtod, DBL_RANGE);
	DO_TEST("0.12", 0.12, -1, strtod, DBL_RANGE);
	DO_TEST("12", 12.0, -1, strtod, DBL_RANGE);
	DO_TEST("12.13", 12.13, -1, strtod, DBL_RANGE);
	DO_TEST("10.0e1", 100.0, -1, strtod, DBL_RANGE);
	DO_TEST("10.0e10", 100000000000.0, -1, strtod, DBL_RANGE);
	DO_TEST("100.0e-1", 10.0, -1, strtod, DBL_RANGE);
	DO_TEST("0x0", 0.0, -1, strtod, DBL_RANGE);
	DO_TEST("0x0.12", 0.0703125, -1, strtod, DBL_RANGE);
	DO_TEST("0x12", 18.0, -1, strtod, DBL_RANGE);
	DO_TEST("0x12.13", 18.07421875, -1, strtod, DBL_RANGE);
	DO_TEST("0x10.0p1", 32.0, -1, strtod, DBL_RANGE);
	DO_TEST("0x10.0p10", 16384.0, -1, strtod, DBL_RANGE);
	DO_TEST("0x100.0p-1", 128.0, -1, strtod, DBL_RANGE);
	DO_TEST_SUCCESS_FUNC("NAN", isnan, -1, strtod);
	DO_TEST_SUCCESS_FUNC("nan", isnan, -1, strtod);
	DO_TEST_SUCCESS_FUNC("INF", isinf, -1, strtod);
	DO_TEST_SUCCESS_FUNC("INFINITY", isinf, -1, strtod);

	DO_TEST("0", 0.0, -1, strtold, LDBL_RANGE);
	DO_TEST("0.12", 0.12, -1, strtold, LDBL_RANGE);
	DO_TEST("12", 12.0, -1, strtold, LDBL_RANGE);
	DO_TEST("12.13", 12.13, -1, strtold, LDBL_RANGE);
	DO_TEST("10.0e1", 100.0, -1, strtold, LDBL_RANGE);
	DO_TEST("10.0e10", 100000000000.0, -1, strtold, LDBL_RANGE);
	DO_TEST("100.0e-1", 10.0, -1, strtold, LDBL_RANGE);
	DO_TEST("0x0", 0.0, -1, strtold, LDBL_RANGE);
	DO_TEST("0x0.12", 0.0703125, -1, strtold, LDBL_RANGE);
	DO_TEST("0x12", 18.0, -1, strtold, LDBL_RANGE);
	DO_TEST("0x12.13", 18.07421875, -1, strtold, LDBL_RANGE);
	DO_TEST("0x10.0p1", 32.0, -1, strtold, LDBL_RANGE);
	DO_TEST("0x10.0p10", 16384.0, -1, strtold, LDBL_RANGE);
	DO_TEST("0x100.0p-1", 128.0, -1, strtold, LDBL_RANGE);
	DO_TEST_SUCCESS_FUNC("NAN", isnan, -1, strtold);
	DO_TEST_SUCCESS_FUNC("nan", isnan, -1, strtold);
	DO_TEST_SUCCESS_FUNC("INF", isinf, -1, strtold);
	DO_TEST_SUCCESS_FUNC("INFINITY", isinf, -1, strtold);

	return 0;
}

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <wctype.h>

int main() {
	char buffer[10];
	int ret = snprintf(buffer, 10, "%d", 123456789);
	assert(strncmp("123456789", buffer, 10) == 0);
	assert(ret == 9);

	// We deliberately induce a warning here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
	ret = snprintf(buffer, 10, "%d", 1234567890);
	assert(strncmp("123456789", buffer, 10) == 0);
	assert(ret == 10);
#pragma GCC diagnostic pop

	// mlibc issue #118.
	ret = snprintf(NULL, 0, "%d", 123456789);
	assert(ret == 9);

	char buf[128];
	assert(snprintf(buf, 2, "a") == 1);
	assert(strcmp(buf, "a") == 0);
	assert(snprintf(buf, 3, "abc") == 3);
	assert(strcmp(buf, "ab") == 0);
	assert(snprintf(buf, 128, "%%") == 1);
	assert(strcmp(buf, "%") == 0);
	assert(snprintf(buf, 128, "a%cc%c", 'b', 'd') == 4);
	assert(strcmp(buf, "abcd") == 0);
	assert(snprintf(buf, 128, "a%lcc%lc", (wint_t)L'b', (wint_t)L'd') == 4);
	assert(strcmp(buf, "abcd") == 0);
	assert(snprintf(buf, 128, "%s", "hello") == 5);
	assert(strcmp(buf, "hello") == 0);
	assert(snprintf(buf, 128, "%ls", L"hello") == 5);
	assert(strcmp(buf, "hello") == 0);

	assert(snprintf(buf, 128, "%d", 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%d", 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%d", -1234) == 5);
	assert(strcmp(buf, "-1234") == 0);
	assert(snprintf(buf, 128, "%hhd", 123) == 3);
	assert(strcmp(buf, "123") == 0);
	assert(snprintf(buf, 128, "%hd", 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%ld", 1234L) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%lld", 1234LL) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%jd", (intmax_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%zd", (ssize_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%td", (ptrdiff_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);

	assert(snprintf(buf, 128, "%i", 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%i", 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%i", -1234) == 5);
	assert(strcmp(buf, "-1234") == 0);
	assert(snprintf(buf, 128, "%hhi", 123) == 3);
	assert(strcmp(buf, "123") == 0);
	assert(snprintf(buf, 128, "%hi", 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%li", 1234L) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%lli", 1234LL) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%ji", (intmax_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%zi", (ssize_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%ti", (ptrdiff_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);

	assert(snprintf(buf, 128, "%b", 0b1011) == 4);
	assert(strcmp(buf, "1011") == 0);
	assert(snprintf(buf, 128, "%b", 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%hhb", 0b1011) == 4);
	assert(strcmp(buf, "1011") == 0);
	assert(snprintf(buf, 128, "%hb", 0b1011) == 4);
	assert(strcmp(buf, "1011") == 0);
	assert(snprintf(buf, 128, "%lb", 0b1011UL) == 4);
	assert(strcmp(buf, "1011") == 0);
	assert(snprintf(buf, 128, "%llb", 0b1011ULL) == 4);
	assert(strcmp(buf, "1011") == 0);
	assert(snprintf(buf, 128, "%jb", (uintmax_t) 0b1011) == 4);
	assert(strcmp(buf, "1011") == 0);
	assert(snprintf(buf, 128, "%zb", (size_t) 0b1011) == 4);
	assert(strcmp(buf, "1011") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%tb", 0b1011UL) == 4);
	assert(strcmp(buf, "1011") == 0);
#pragma GCC diagnostic pop

	assert(snprintf(buf, 128, "%o", 01234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%o", 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%o", -01234) == 11);
	assert(strcmp(buf, "37777776544") == 0);
	assert(snprintf(buf, 128, "%hho", 012) == 2);
	assert(strcmp(buf, "12") == 0);
	assert(snprintf(buf, 128, "%ho", 01234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%lo", 01234UL) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%llo", 01234ULL) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%jo", (uintmax_t) 01234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%zo", (size_t) 01234) == 4);
	assert(strcmp(buf, "1234") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%to", 01234UL) == 4);
	assert(strcmp(buf, "1234") == 0);
#pragma GCC diagnostic pop

	assert(snprintf(buf, 128, "%x", 0x1234AB) == 6);
	assert(strcmp(buf, "1234ab") == 0);
	assert(snprintf(buf, 128, "%x", 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%x", -0x1234AB) == 8);
	assert(strcmp(buf, "ffedcb55") == 0);
	assert(snprintf(buf, 128, "%X", 0x1234AB) == 6);
	assert(strcmp(buf, "1234AB") == 0);
	assert(snprintf(buf, 128, "%hhx", 0xAB) == 2);
	assert(strcmp(buf, "ab") == 0);
	assert(snprintf(buf, 128, "%hx", 0x12AB) == 4);
	assert(strcmp(buf, "12ab") == 0);
	assert(snprintf(buf, 128, "%lx", 0x1234ABUL) == 6);
	assert(strcmp(buf, "1234ab") == 0);
	assert(snprintf(buf, 128, "%llx", 0x1234ABULL) == 6);
	assert(strcmp(buf, "1234ab") == 0);
	assert(snprintf(buf, 128, "%jx", (uintmax_t) 0x1234AB) == 6);
	assert(strcmp(buf, "1234ab") == 0);
	assert(snprintf(buf, 128, "%zx", (size_t) 0x1234AB) == 6);
	assert(strcmp(buf, "1234ab") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%tx", 0x1234ABUL) == 6);
	assert(strcmp(buf, "1234ab") == 0);
#pragma GCC diagnostic pop

	assert(snprintf(buf, 128, "%u", 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%u", 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%u", -1234) == 10);
	assert(strcmp(buf, "4294966062") == 0);
	assert(snprintf(buf, 128, "%hhu", 123) == 3);
	assert(strcmp(buf, "123") == 0);
	assert(snprintf(buf, 128, "%hu", 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%lu", 1234UL) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%llu", 1234ULL) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%ju", (uintmax_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
	assert(snprintf(buf, 128, "%zu", (size_t) 1234) == 4);
	assert(strcmp(buf, "1234") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%tu", 1234UL) == 4);
	assert(strcmp(buf, "1234") == 0);
#pragma GCC diagnostic pop

	assert(snprintf(buf, 128, "%f", 1234.1234567) == 11);
	assert(strcmp(buf, "1234.123457") == 0);
	assert(snprintf(buf, 128, "%f", 0.0) == 8);
	assert(strcmp(buf, "0.000000") == 0);
	assert(snprintf(buf, 128, "%f", -1234.123456) == 12);
	assert(strcmp(buf, "-1234.123456") == 0);
	assert(snprintf(buf, 128, "%f", INFINITY) == 3);
	assert(strcmp(buf, "inf") == 0);
	assert(snprintf(buf, 128, "%04f", INFINITY) == 4);
	assert(strcmp(buf, " inf") == 0);
	assert(snprintf(buf, 128, "%-4f", INFINITY) == 4);
	assert(strcmp(buf, "inf ") == 0);
	assert(snprintf(buf, 128, "%f", NAN) == 3);
	assert(strcmp(buf, "nan") == 0);
	assert(snprintf(buf, 128, "%lf", 1234.123456) == 11);
	assert(strcmp(buf, "1234.123456") == 0);
	assert(snprintf(buf, 128, "%Lf", 1234.123456L) == 11);
	assert(strcmp(buf, "1234.123456") == 0);
	assert(snprintf(buf, 128, "%F", 1234.123456) == 11);
	assert(strcmp(buf, "1234.123456") == 0);
	assert(snprintf(buf, 128, "%F", INFINITY) == 3);
	assert(strcmp(buf, "INF") == 0);
	assert(snprintf(buf, 128, "%F", NAN) == 3);
	assert(strcmp(buf, "NAN") == 0);
	assert(snprintf(buf, 128, "%lF", 1234.123456) == 11);
	assert(strcmp(buf, "1234.123456") == 0);
	assert(snprintf(buf, 128, "%LF", 1234.123456L) == 11);
	assert(strcmp(buf, "1234.123456") == 0);

	assert(snprintf(buf, 128, "%e", 1.123456) == 12);
	assert(strcmp(buf, "1.123456e+00") == 0);
	assert(snprintf(buf, 128, "%e", 0.0) == 12);
	assert(strcmp(buf, "0.000000e+00") == 0);
	assert(snprintf(buf, 128, "%e", -1.123456) == 13);
	assert(strcmp(buf, "-1.123456e+00") == 0);
	assert(snprintf(buf, 128, "%e", 1.123456e12) == 12);
	assert(strcmp(buf, "1.123456e+12") == 0);
	assert(snprintf(buf, 128, "%e", -1.123456e12) == 13);
	assert(strcmp(buf, "-1.123456e+12") == 0);
	assert(snprintf(buf, 128, "%e", -1.123456e-12) == 13);
	assert(strcmp(buf, "-1.123456e-12") == 0);
	assert(snprintf(buf, 128, "%e", INFINITY) == 3);
	assert(strcmp(buf, "inf") == 0);
	assert(snprintf(buf, 128, "%e", NAN) == 3);
	assert(strcmp(buf, "nan") == 0);
	assert(snprintf(buf, 128, "%le", 1.123456e12) == 12);
	assert(strcmp(buf, "1.123456e+12") == 0);
	assert(snprintf(buf, 128, "%Le", 1.123456e12L) == 12);
	assert(strcmp(buf, "1.123456e+12") == 0);
	assert(snprintf(buf, 128, "%E", 1.123456e12) == 12);
	assert(strcmp(buf, "1.123456E+12") == 0);
	assert(snprintf(buf, 128, "%E", INFINITY) == 3);
	assert(strcmp(buf, "INF") == 0);
	assert(snprintf(buf, 128, "%E", NAN) == 3);
	assert(strcmp(buf, "NAN") == 0);
	assert(snprintf(buf, 128, "%lE", 1.123456e12) == 12);
	assert(strcmp(buf, "1.123456E+12") == 0);
	assert(snprintf(buf, 128, "%LE", 1.123456e12L) == 12);
	assert(strcmp(buf, "1.123456E+12") == 0);

	/*assert(snprintf(buf, 128, "%a", 0x1.123456p0) == 13);
	assert(strcmp(buf, "0x1.123456p+0") == 0);
	assert(snprintf(buf, 128, "%a", 0.0) == 6);
	assert(strcmp(buf, "0x0p+0") == 0);
	assert(snprintf(buf, 128, "%a", 0x1.123456p12) == 14);
	assert(strcmp(buf, "0x1.123456p+12") == 0);
	assert(snprintf(buf, 128, "%a", -0x1.123456p-12) == 15);
	assert(strcmp(buf, "-0x1.123456p-12") == 0);
	assert(snprintf(buf, 128, "%a", INFINITY) == 3);
	assert(strcmp(buf, "inf") == 0);
	assert(snprintf(buf, 128, "%a", NAN) == 3);
	assert(strcmp(buf, "nan") == 0);
	assert(snprintf(buf, 128, "%la", 0x1.123456p12) == 14);
	assert(strcmp(buf, "0x1.123456p+12") == 0);
	assert(snprintf(buf, 128, "%La", 0x1.123456p12L) == 12);
	assert(strcmp(buf, "0x8.91a2bp+9") == 0);
	assert(snprintf(buf, 128, "%A", 0x1.123456p12) == 14);
	assert(strcmp(buf, "0X1.123456P+12") == 0);
	assert(snprintf(buf, 128, "%A", INFINITY) == 3);
	assert(strcmp(buf, "INF") == 0);
	assert(snprintf(buf, 128, "%A", NAN) == 3);
	assert(strcmp(buf, "NAN") == 0);
	assert(snprintf(buf, 128, "%lA", 0x1.123456p12) == 14);
	assert(strcmp(buf, "0X1.123456P+12") == 0);
	assert(snprintf(buf, 128, "%LA", 0x1.123456p12L) == 12);
	assert(strcmp(buf, "0X8.91A2BP+9") == 0);*/

	assert(snprintf(buf, 128, "%g", 1.1234) == 6);
	assert(strcmp(buf, "1.1234") == 0);
	assert(snprintf(buf, 128, "%g", 0.0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%g", -1.1234) == 7);
	assert(strcmp(buf, "-1.1234") == 0);
	assert(snprintf(buf, 128, "%g", 1.1234e12) == 10);
	assert(strcmp(buf, "1.1234e+12") == 0);
	assert(snprintf(buf, 128, "%g", -1.1234e-12) == 11);
	assert(strcmp(buf, "-1.1234e-12") == 0);
	assert(snprintf(buf, 128, "%g", INFINITY) == 3);
	assert(strcmp(buf, "inf") == 0);
	assert(snprintf(buf, 128, "%g", NAN) == 3);
	assert(strcmp(buf, "nan") == 0);
	assert(snprintf(buf, 128, "%lg", 1.1234e12) == 10);
	assert(strcmp(buf, "1.1234e+12") == 0);
	assert(snprintf(buf, 128, "%Lg", 1.1234e12L) == 10);
	assert(strcmp(buf, "1.1234e+12") == 0);
	assert(snprintf(buf, 128, "%G", 1.1234) == 6);
	assert(strcmp(buf, "1.1234") == 0);
	assert(snprintf(buf, 128, "%G", 1.1234e12) == 10);
	assert(strcmp(buf, "1.1234E+12") == 0);
	assert(snprintf(buf, 128, "%G", INFINITY) == 3);
	assert(strcmp(buf, "INF") == 0);
	assert(snprintf(buf, 128, "%G", NAN) == 3);
	assert(strcmp(buf, "NAN") == 0);
	assert(snprintf(buf, 128, "%lG", 1.1234e12) == 10);
	assert(strcmp(buf, "1.1234E+12") == 0);
	assert(snprintf(buf, 128, "%LG", 1.1234e12L) == 10);
	assert(strcmp(buf, "1.1234E+12") == 0);

	signed char char_value = 0;
	assert(snprintf(buf, 128, "ab%hhnc", &char_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(char_value == 2);
	short short_value = 0;
	assert(snprintf(buf, 128, "ab%hnc", &short_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(short_value == 2);
	int int_value = 0;
	assert(snprintf(buf, 128, "ab%nc", &int_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(int_value == 2);
	long long_value = 0;
	assert(snprintf(buf, 128, "ab%lnc", &long_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(long_value == 2);
	long long long_long_value = 0;
	assert(snprintf(buf, 128, "ab%llnc", &long_long_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(long_long_value == 2);
	intmax_t intmax_value = 0;
	assert(snprintf(buf, 128, "ab%jnc", &intmax_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(intmax_value == 2);
	ssize_t size_value = 0;
	assert(snprintf(buf, 128, "ab%znc", &size_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(size_value == 2);
	ptrdiff_t ptrdiff_value = 0;
	assert(snprintf(buf, 128, "ab%tnc", &ptrdiff_value) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(ptrdiff_value == 2);

#if UINTPTR_MAX == UINT64_MAX
	uintptr_t ptr_value = 0xFFFFFFFFFFFFFFFF;
	assert(snprintf(buf, 128, "%p", (void *)ptr_value) == 18);
	assert(strcmp(buf, "0xffffffffffffffff") == 0);
#else
	uintptr_t ptr_value = 0xFFFFFFFF;
	assert(snprintf(buf, 128, "%p", (void*)ptr_value) == 10);
	assert(strcmp(buf, "0xffffffff") == 0);
#endif

	ptr_value = 0xFFFF;
	assert(snprintf(buf, 128, "%p", (void*)ptr_value) == 6);
	assert(strcmp(buf, "0xffff") == 0);

	assert(snprintf(buf, 128, "%-4dhello", 12) == 9);
	assert(strcmp(buf, "12  hello") == 0);

	assert(snprintf(buf, 128, "%-4dhello", -12) == 9);
	assert(strcmp(buf, "-12 hello") == 0);

	assert(snprintf(buf, 128, "%#-4xhello", 0x12) == 9);
	assert(strcmp(buf, "0x12hello") == 0);
	assert(snprintf(buf, 128, "%#-4Xhello", 0x12) == 9);
	assert(strcmp(buf, "0X12hello") == 0);

	assert(snprintf(buf, 128, "%#b", 0b1011) == 6);
	assert(strcmp(buf, "0b1011") == 0);
	assert(snprintf(buf, 128, "%#B", 0b1011) == 6);
	assert(strcmp(buf, "0B1011") == 0);

	assert(snprintf(buf, 128, "%+d", 1234) == 5);
	assert(strcmp(buf, "+1234") == 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%+ d", 1234) == 5);
	assert(strcmp(buf, "+1234") == 0);
#pragma GCC diagnostic pop

	assert(snprintf(buf, 128, "% d", 1234) == 5);
	assert(strcmp(buf, " 1234") == 0);

	assert(snprintf(buf, 128, "%4dhello", 12) == 9);
	assert(strcmp(buf, "  12hello") == 0);
	assert(snprintf(buf, 128, "%2dhello", 1234) == 9);
	assert(strcmp(buf, "1234hello") == 0);
	int width = 4;
	assert(snprintf(buf, 128, "%*dhello", width, 12) == 9);
	assert(strcmp(buf, "  12hello") == 0);
	width = -4;
	assert(snprintf(buf, 128, "%*dhello", width, 12) == 9);
	assert(strcmp(buf, "12  hello") == 0);

	assert(snprintf(buf, 128, "%04dhello", 12) == 9);
	assert(strcmp(buf, "0012hello") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%-04dhello", 12) == 9);
	assert(strcmp(buf, "12  hello") == 0);
	assert(snprintf(buf, 128, "%04.2dhello", 1) == 9);
	assert(strcmp(buf, "  01hello") == 0);
#pragma GCC diagnostic pop
	assert(snprintf(buf, 128, "%.d", 0) == 0);
	assert(strcmp(buf, "") == 0);
	assert(snprintf(buf, 128, "%.0d", 0) == 0);
	assert(strcmp(buf, "") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(snprintf(buf, 128, "%04.d", 0) == 4);
	assert(strcmp(buf, "    ") == 0);
	assert(snprintf(buf, 128, "%#04.x", 0) == 4);
	assert(strcmp(buf, "    ") == 0);
	assert(snprintf(buf, 128, "%+04.d", 0) == 4);
	assert(strcmp(buf, "   +") == 0);
	assert(snprintf(buf, 128, "%+0.d", 0) == 1);
	assert(strcmp(buf, "+") == 0);
#pragma GCC diagnostic pop
	int precision = 0;
	assert(snprintf(buf, 128, "%.*d", precision, 0) == 0);
	assert(strcmp(buf, "") == 0);
	precision = -1;
	assert(snprintf(buf, 128, "%.*d", precision, 0) == 1);
	assert(strcmp(buf, "0") == 0);
	assert(snprintf(buf, 128, "%.6d", 1234) == 6);
	assert(strcmp(buf, "001234") == 0);

	assert(snprintf(buf, 128, "%.o", 0) == 0);
	assert(strcmp(buf, "") == 0);
	assert(snprintf(buf, 128, "%.0o", 0) == 0);
	assert(strcmp(buf, "") == 0);

	return 0;
}

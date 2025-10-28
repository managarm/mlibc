#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

struct format_test_cases {
	const char *format;
	const char *data;
	int expected_int;
	enum {
		T_INT,
		T_UINT,
		T_CHAR,
		T_NONE,
	} type;
	int ret;
} formats[] = {
	{"%i", "0x420", 0x420, T_INT, 1},
	{"%i", "0420", 0420, T_INT, 1},
	{"%i", "420", 420, T_INT, 1},
	{"%i", "-420", -420, T_INT, 1},
	{"%d", "-12345", -12345, T_INT, 1},
	{"%u", "69", 69, T_UINT, 1},
	{"%u", "0420", 420, T_UINT, 1},
	{"%o", "0420", 0420, T_UINT, 1},
	{"%x", "0xCB7", 0xCB7, T_UINT, 1},
#ifndef USE_HOST_LIBC
	{"%b", "0b1011", 0b1011, T_UINT, 1},
	{"%b", "0B1011", 0b1011, T_UINT, 1},
#endif
	{"%%", "%", 0, T_NONE, 0},
	{"%c", "         I am not a fan of this solution.", ' ', T_CHAR, 1},
	{" %c", "           CBT (capybara therapy)", 'C', T_CHAR, 1},
	{"%d %d %d", "  111111 I<3Managarm 1234", 111111, T_UINT, 1},
	{"%c %d", "C", 'C', T_CHAR, 1}
};

#pragma GCC diagnostic ignored "-Wformat-security"

static void test_matrix() {
	for(size_t i = 0; i < (sizeof(formats) / sizeof(*formats)); i++) {
		struct format_test_cases *f = &formats[i];
		int ret = -1;
		int data_int;
		unsigned int data_uint;
		char data_char;

		switch(f->type) {
			case T_INT: {
				ret = sscanf(f->data, f->format, &data_int);
				assert(data_int == f->expected_int);
				break;
			}
			case T_UINT: {
				ret = sscanf(f->data, f->format, &data_uint);
				assert(data_uint == (unsigned int) f->expected_int);
				break;
			}
			case T_CHAR: {
				ret = sscanf(f->data, f->format, &data_char);
				assert(data_char == (unsigned char) f->expected_int);
				break;
			}
			case T_NONE: {
				ret = sscanf(f->data, f->format);
				break;
			}

		}

		assert(ret == f->ret);
	}
}

int main() {
	{
		int x = 0;
		char buf[] = "12345";
		sscanf(buf, "%d", &x);
		assert(x == 12345);
	}

	{
		char c;
		int n1;
		int n2;
		char buf[] = "z$ 7 5 440";;
		int count = sscanf(buf, "%*c%c %d %*d %d", &c, &n1, &n2);
		assert(count == 3);
		assert(c == '$');
		assert(n1 == 7);
		assert(n2 == 440);
	}

	{
		// From dsda-doom
		char buf[] = "process_priority               0\n";
		char def[80], strparm[128];
		memset(def, '!', 80);
		memset(strparm, '!', 128);
		sscanf(buf, "%s %[^\n]\n", def, strparm);
		assert(!strcmp(def, "process_priority"));
		assert(!strcmp(strparm, "0"));
	}

	{
		char buf[] = "fffff100";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0xfffff100);
	}

#if !defined(__i386__) && !defined(__m68k__)
	{
		char buf[] = "fffffffff100";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0xfffffffff100);
	}
#endif

	{
		char buf[] = "410dc000";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0x410dc000);
	}

	{
		// From webkitgtk
		char buf[] = "MemTotal:       16299664 kB\n";
		char token[51] = {0};
		size_t amount = 0;
		int ret = sscanf(buf, "%50s%zukB", token, &amount);
		assert(ret == 2);
		assert(!strcmp(token, "MemTotal:"));
		assert(amount == 16299664);
	}

	{
		char buf[] = "SIGINT";
		int sig;
		int ret = sscanf(buf, "%d", &sig);
		assert(!ret);
	}

	{
		char buf[50];
		int ret = sscanf("", "%s", buf);
		assert(ret == EOF);
	}

	{
		char *str = NULL;
		int ret = sscanf("Managarm", "%ms", &str);
		assert(ret == 1);
		assert(str != NULL);
		assert(!strcmp(str, "Managarm"));
		free(str);
	}

	{
		// From openjdk
		char buf[] = "SomeOption=someValue";
		char name[256];
		char punct;
		int ret = sscanf(buf, "%255[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]%c", name, &punct);
		assert(ret == 2);
		assert(punct == '=');
		assert(!strcmp(name, "SomeOption"));
	}

	test_matrix();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
	assert(sscanf("", "") == 0);
#pragma GCC diagnostic pop
	assert(sscanf("", "a") == EOF);
	assert(sscanf(" \t\na", " a") == 0);
	assert(sscanf("a%", "a%%") == 0);
	assert(sscanf("ab", "abcd") == EOF);
	assert(sscanf("abab", "abcd") == 0);

	char char_value[8];
	wchar_t wchar_value[8];

	assert(sscanf("a", "%c", char_value) == 1);
	assert(char_value[0] == 'a');
	assert(sscanf("a", "%lc", wchar_value) == 1);
	assert(wchar_value[0] == L'a');
	assert(sscanf("hello", "%s", char_value) == 1);
	assert(strcmp(char_value, "hello") == 0);
	assert(sscanf("", "%s", char_value) == EOF);
	assert(sscanf(" ", "%s", char_value) == EOF);
	assert(sscanf(" a", "%sb", char_value) == 1);
	assert(sscanf("hello", "%ls", wchar_value) == 1);
	assert(wcscmp(wchar_value, L"hello") == 0);
	assert(sscanf("abcd", "%[ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "a") == 0);
	assert(sscanf("abcd", "%l[ac]bcd", wchar_value) == 1);
	assert(wcscmp(wchar_value, L"a") == 0);
	assert(sscanf("aacd", "%[ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "aac") == 0);
	assert(sscanf("aacd", "%[ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "aac") == 0);
	assert(sscanf("]", "%[]]", char_value) == 1);
	assert(strcmp(char_value, "]") == 0);
	assert(sscanf("]", "%[^]]", char_value) == 0);
	assert(sscanf("abcd", "%[^ac]bcd", char_value) == 0);
	assert(sscanf("aacd", "%[^ac]bcd", char_value) == 0);
	assert(sscanf("aacd", "%[^ac]bcd", char_value) == 0);
	assert(sscanf("ddd", "%[^ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "ddd") == 0);
	assert(sscanf("abcd", "%[0-9]", char_value) == 0);
	assert(sscanf("2a", "%[0-9]", char_value) == 1);
	assert(strcmp(char_value, "2") == 0);

	unsigned char uchar_value;
	signed char schar_value;
	unsigned short ushort_value;
	short short_value;
	unsigned int uint_value;
	int int_value;
	unsigned long ulong_value;
	long long_value;
	unsigned long long ulonglong_value;
	long long longlong_value;
	uintmax_t uintmax_value;
	intmax_t intmax_value;
	size_t size_value;
	ptrdiff_t ptrdiff_value;

	assert(sscanf("1234abc", "%d", &int_value) == 1);
	assert(int_value == 1234);
	assert(sscanf("123", "%hhd", &schar_value) == 1);
	assert(schar_value == 123);
	assert(sscanf("1234", "%hd", &short_value) == 1);
	assert(short_value == 1234);
	assert(sscanf("1234", "%ld", &long_value) == 1);
	assert(long_value == 1234);
	assert(sscanf("1234", "%lld", &longlong_value) == 1);
	assert(longlong_value == 1234);
	assert(sscanf("1234", "%jd", &intmax_value) == 1);
	assert(intmax_value == 1234);
	assert(sscanf("1234", "%zd", &size_value) == 1);
	assert(size_value == 1234);
	assert(sscanf("1234", "%td", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 1234);
	assert(sscanf("    1235abc", "%d", &int_value) == 1);
	assert(int_value == 1235);
	assert(sscanf("+1234abc", "%d", &int_value) == 1);
	assert(int_value == 1234);
	assert(sscanf("-1234abc", "%d", &int_value) == 1);
	assert(int_value == -1234);
	assert(sscanf("-abc", "%d", &int_value) == 0);
	assert(sscanf("-", "%d", &int_value) == 0);

	assert(sscanf("1234abc", "%i", &int_value) == 1);
	assert(int_value == 1234);
	assert(sscanf("123", "%hhi", &schar_value) == 1);
	assert(schar_value == 123);
	assert(sscanf("1234", "%hi", &short_value) == 1);
	assert(short_value == 1234);
	assert(sscanf("1234", "%li", &long_value) == 1);
	assert(long_value == 1234);
	assert(sscanf("1234", "%lli", &longlong_value) == 1);
	assert(longlong_value == 1234);
	assert(sscanf("1234", "%ji", &intmax_value) == 1);
	assert(intmax_value == 1234);
	assert(sscanf("1234", "%zi", &size_value) == 1);
	assert(size_value == 1234);
	assert(sscanf("1234", "%ti", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 1234);
	assert(sscanf("    1235abc", "%i", &int_value) == 1);
	assert(int_value == 1235);
	assert(sscanf("+1234abc", "%i", &int_value) == 1);
	assert(int_value == 1234);
	assert(sscanf("-1234abc", "%i", &int_value) == 1);
	assert(int_value == -1234);
	assert(sscanf("-abc", "%i", &int_value) == 0);

	assert(sscanf(" 0b1011", "%i", &int_value) == 1);
	assert(int_value == 0b1011);
	assert(sscanf(" 0B1010", "%i", &int_value) == 1);
	assert(int_value == 0b1010);
	assert(sscanf(" +0b1011", "%i", &int_value) == 1);
	assert(int_value == 0b1011);
	assert(sscanf(" -0b1111", "%i", &int_value) == 1);
	assert(int_value == -0b1111);

	assert(sscanf(" 01234", "%i", &int_value) == 1);
	assert(int_value == 01234);
	assert(sscanf(" +01234", "%i", &int_value) == 1);
	assert(int_value == 01234);
	assert(sscanf(" -01234", "%i", &int_value) == 1);
	assert(int_value == -01234);

	assert(sscanf(" 0xab12", "%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(sscanf(" 0xAB12", "%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(sscanf(" 0Xab12", "%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(sscanf(" +0xab12", "%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(sscanf(" -0xab12", "%i", &int_value) == 1);
	assert(int_value == -0xAB12);

	assert(sscanf("1234abc", "%u", &uint_value) == 1);
	assert(uint_value == 1234);
	assert(sscanf("123", "%hhu", &uchar_value) == 1);
	assert(uchar_value == 123);
	assert(sscanf("1234", "%hu", &ushort_value) == 1);
	assert(ushort_value == 1234);
	assert(sscanf("1234", "%lu", &ulong_value) == 1);
	assert(ulong_value == 1234);
	assert(sscanf("1234", "%llu", &ulonglong_value) == 1);
	assert(ulonglong_value == 1234);
	assert(sscanf("1234", "%ju", &uintmax_value) == 1);
	assert(uintmax_value == 1234);
	assert(sscanf("1234", "%zu", &size_value) == 1);
	assert(size_value == 1234);
	assert(sscanf("1234", "%tu", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 1234);
	assert(sscanf("    1235abc", "%u", &int_value) == 1);
	assert(int_value == 1235);
	assert(sscanf("+1234abc", "%u", &int_value) == 1);
	assert(int_value == 1234);
	assert(sscanf("-1234abc", "%u", &int_value) == 1);
	assert(int_value == -1234);
	assert(sscanf("-abc", "%d", &int_value) == 0);

	assert(sscanf(" 12345", "%o", &int_value) == 1);
	assert(int_value == 012345);
	assert(sscanf("12", "%hho", &uchar_value) == 1);
	assert(uchar_value == 012);
	assert(sscanf("1234", "%ho", &ushort_value) == 1);
	assert(ushort_value == 01234);
	assert(sscanf("1234", "%lo", &ulong_value) == 1);
	assert(ulong_value == 01234);
	assert(sscanf("1234", "%llo", &ulonglong_value) == 1);
	assert(ulonglong_value == 01234);
	assert(sscanf("1234", "%jo", &uintmax_value) == 1);
	assert(uintmax_value == 01234);
	assert(sscanf("1234", "%zo", &size_value) == 1);
	assert(size_value == 01234);
	assert(sscanf("1234", "%to", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 01234);
	assert(sscanf(" 01234", "%o", &int_value) == 1);
	assert(int_value == 01234);
	assert(sscanf(" +01234", "%o", &int_value) == 1);
	assert(int_value == 01234);
	assert(sscanf(" -01234", "%o", &int_value) == 1);
	assert(int_value == -01234);

	assert(sscanf(" 1234ab", "%x", &int_value) == 1);
	assert(int_value == 0x1234AB);
	assert(sscanf("12", "%hhx", &uchar_value) == 1);
	assert(uchar_value == 0x12);
	assert(sscanf("1234", "%hx", &ushort_value) == 1);
	assert(ushort_value == 0x1234);
	assert(sscanf("1234", "%lx", &ulong_value) == 1);
	assert(ulong_value == 0x1234);
	assert(sscanf("1234", "%llx", &ulonglong_value) == 1);
	assert(ulonglong_value == 0x1234);
	assert(sscanf("1234", "%jx", &uintmax_value) == 1);
	assert(uintmax_value == 0x1234);
	assert(sscanf("1234", "%zx", &size_value) == 1);
	assert(size_value == 0x1234);
	assert(sscanf("1234", "%tx", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 0x1234);
	assert(sscanf(" 1234ABC", "%x", &int_value) == 1);
	assert(int_value == 0x1234ABC);
	assert(sscanf(" 1234ab", "%X", &int_value) == 1);
	assert(int_value == 0x1234AB);
	assert(sscanf(" 0x1234", "%x", &int_value) == 1);
	assert(int_value == 0x1234);
	assert(sscanf(" 0x1234A", "%x", &int_value) == 1);
	assert(int_value == 0x1234A);
	assert(sscanf(" 0X1234AB", "%x", &int_value) == 1);
	assert(int_value == 0x1234AB);
	assert(sscanf(" +0x1234", "%x", &int_value) == 1);
	assert(int_value == 0x1234);
	assert(sscanf(" -0x1234", "%x", &int_value) == 1);
	assert(int_value == -0x1234);

	assert(sscanf("abc", "abc%n", &int_value) == 0);
	assert(int_value == 3);
	assert(sscanf("1234", "1234%hhn", &uchar_value) == 0);
	assert(uchar_value == 4);
	assert(sscanf("1234", "1234%hn", &ushort_value) == 0);
	assert(ushort_value == 4);
	assert(sscanf("1234", "1234%ln", &ulong_value) == 0);
	assert(ulong_value == 4);
	assert(sscanf("1234", "1234%lln", &ulonglong_value) == 0);
	assert(ulonglong_value == 4);
	assert(sscanf("1234", "1234%jn", &uintmax_value) == 0);
	assert(uintmax_value == 4);
	assert(sscanf("1234", "1234%zn", &size_value) == 0);
	assert(size_value == 4);
	assert(sscanf("1234", "1234%tn", &ptrdiff_value) == 0);
	assert(ptrdiff_value == 4);
	assert(sscanf("abcd", "ab%nc%c", &int_value, char_value) == 1);
	assert(int_value == 2);
	assert(char_value[0] == 'd');

	float float_value;
	double double_value;
	long double long_double_value;
	assert(sscanf("1.123", "%a", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(sscanf("1.123", "%la", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(sscanf("1.123", "%La", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(sscanf("1.1234", "%A", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(sscanf("1.123", "%e", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(sscanf("1.123", "%le", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(sscanf("1.123", "%Le", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(sscanf("1.1234", "%E", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(sscanf("1.123", "%f", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(sscanf("1.123", "%lf", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(sscanf("1.123", "%Lf", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(sscanf("1.1234", "%F", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(sscanf("1.123", "%g", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(sscanf("1.123", "%lg", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(sscanf("1.123", "%Lg", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(sscanf("1.1234", "%G", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(sscanf("+1.1234", "%f", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(sscanf("-1.1234", "%f", &float_value) == 1);
	assert(float_value >= -1.1234f - 0.01 && float_value <= -1.1234f + 0.01);
	assert(sscanf("1.1234e3", "%f", &float_value) == 1);
	assert(float_value >= 1.1234E3f - 0.01 && float_value <= 1.1234E3f + 0.01);
	assert(sscanf("1.1234E3", "%f", &float_value) == 1);
	assert(float_value >= 1.1234E3f - 0.01 && float_value <= 1.1234E3f + 0.01);
	assert(sscanf("0x1234", "%f", &float_value) == 1);
	assert(float_value >= 0x1234P0f - 0.01 && float_value <= 0x1234P0f + 0.01);
	assert(sscanf("0X1234", "%f", &float_value) == 1);
	assert(float_value >= 0x1234P0f - 0.01 && float_value <= 0x1234P0f + 0.01);
	assert(sscanf("0x1.1234", "%f", &float_value) == 1);
	assert(float_value >= 0x1.1234P0 - 0.01 && float_value <= 0x1.1234P0 + 0.01);
	assert(sscanf("0x1.abcdP3", "%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP3 - 0.01 && float_value <= 0x1.ABCDP3 + 0.01);
	assert(sscanf("0x1.abcdP+3", "%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP3 - 0.01 && float_value <= 0x1.ABCDP3 + 0.01);
	assert(sscanf("0x1.81c8P+13", "%f", &float_value) == 1);
	assert(float_value >= 12345.0 - 0.01 && float_value <= 12345.0 + 0.01);
	assert(sscanf("0x1.abcdP-3", "%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP-3 - 0.01 && float_value <= 0x1.ABCDP-3 + 0.01);
	assert(sscanf("0x1.AbCdP-3", "%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP-3 - 0.01 && float_value <= 0x1.ABCDP-3 + 0.01);
	assert(sscanf("inf", "%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(sscanf("InF", "%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(sscanf("+inf", "%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(sscanf("-inf", "%f", &float_value) == 1);
	assert(isinf(float_value) && __builtin_signbit(float_value));
	assert(sscanf("INF", "%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(sscanf("infinity", "%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(sscanf("INFINITY", "%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(sscanf("-infe", "%f", &float_value) == 1);
	assert(isinf(float_value) && __builtin_signbit(float_value));
	assert(sscanf("nan", "%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(sscanf("NaN", "%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(sscanf("naP", "%f", &float_value) == 0);
	assert(sscanf("+nan", "%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(sscanf("-nan", "%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(sscanf("-nan", "%Lf", &long_double_value) == 1);
	assert(isnan(long_double_value));
	assert(sscanf("0xhello", "%f", &float_value) == 0);
	assert(sscanf("0x1hello", "%f", &float_value) == 1);
	assert(float_value >= 0x1P0f - 0.01 && float_value <= 0x1P0f + 0.01);
	assert(sscanf("ab1.1234", "%f", &float_value) == 0);
	assert(sscanf("  1.123", "%f", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(sscanf(".1", "%f", &float_value) == 1);
	assert(float_value >= .1f - 0.01 && float_value <= .1f + 0.01);

	void* ptr;
	assert(sscanf("hello", "%p", &ptr) == 0);
#if UINTPTR_MAX == UINT64_MAX
	assert(sscanf("0xabcdef12abcdef12", "%p", &ptr) == 1);
	assert(ptr == (void*)0xabcdef12abcdef12);
#else
	assert(sscanf("0xabcdef12", "%p", &ptr) == 1);
	assert(ptr == (void*)0xabcdef12);
#endif

	assert(sscanf("a", "%*c") == 0);
	char_value[2] = 'q';
	assert(sscanf("abcd", "%2c", char_value) == 1);
	assert(char_value[2] == 'q');
	char_value[2] = 0;
	assert(strcmp(char_value, "ab") == 0);

	assert(sscanf("cdef", "%2s", char_value) == 1);
	assert(strcmp(char_value, "cd") == 0);
	assert(sscanf("c def", "%2s", char_value) == 1);
	assert(strcmp(char_value, "c") == 0);

	assert(sscanf("aacd", "%2[ac]", char_value) == 1);
	assert(strcmp(char_value, "aa") == 0);

	assert(sscanf("zz-zxx-mmm", "%7[zx-]", char_value) == 1);
	assert(strcmp(char_value, "zz-zxx-") == 0);

	return 0;
}

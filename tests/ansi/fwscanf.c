#include <locale.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

struct format_test_cases {
	const wchar_t *format;
	const wchar_t *data;
	int expected_int;
	enum {
		T_INT,
		T_UINT,
		T_CHAR,
		T_WCHAR,
		T_NONE,
	} type;
	int ret;
} formats[] = {
	{L"%i", L"0x420", 0x420, T_INT, 1},
	{L"%i", L"0420", 0420, T_INT, 1},
	{L"%i", L"420", 420, T_INT, 1},
	{L"%i", L"-420", -420, T_INT, 1},
	{L"%d", L"-12345", -12345, T_INT, 1},
	{L"%u", L"69", 69, T_UINT, 1},
	{L"%u", L"0420", 420, T_UINT, 1},
	{L"%o", L"0420", 0420, T_UINT, 1},
	{L"%x", L"0xCB7", 0xCB7, T_UINT, 1},
#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	{L"%b", L"0b1011", 0b1011, T_UINT, 1},
	{L"%b", L"0B1011", 0b1011, T_UINT, 1},
#endif
	{L"%%", L"%", 0, T_NONE, 0},
	{L"%c", L"         I am not a fan of this solution.", ' ', T_CHAR, 1},
	{L" %c", L"           CBT (capybara therapy)", 'C', T_CHAR, 1},
	{L"%lc", L"         I am not a fan of this solution.", L' ', T_WCHAR, 1},
	{L" %lc", L"           CBT (capybara therapy)", L'C', T_WCHAR, 1},
	{L"%d %d %d", L"  111111 I<3Managarm 1234", 111111, T_UINT, 1},
	{L"%c %d", L"C", 'C', T_CHAR, 1}
};

#pragma GCC diagnostic ignored "-Wformat-security"

static void test_matrix() {
	for(size_t i = 0; i < (sizeof(formats) / sizeof(*formats)); i++) {
		struct format_test_cases *f = &formats[i];
		int ret = -1;
		int data_int;
		unsigned int data_uint;
		char data_char;
		wchar_t data_wchar;

		FILE *fp = tmpfile();
		assert(fp);
		assert(fputws(f->data, fp) >= 0);
		rewind(fp);

		switch(f->type) {
			case T_INT: {
				ret = fwscanf(fp, f->format, &data_int);
				assert(data_int == f->expected_int);
				break;
			}
			case T_UINT: {
				ret = fwscanf(fp, f->format, &data_uint);
				assert(data_uint == (unsigned int) f->expected_int);
				break;
			}
			case T_CHAR: {
				ret = fwscanf(fp, f->format, &data_char);
				assert((unsigned char) data_char == (unsigned char) f->expected_int);
				break;
			}
			case T_WCHAR: {
				ret = fwscanf(fp, f->format, &data_wchar);
				assert((wint_t) data_wchar == (wint_t) f->expected_int);
				break;
			}
			case T_NONE: {
				ret = fwscanf(fp, f->format);
				break;
			}
		}

		assert(ret == f->ret);

		fclose(fp);
	}
}

int main() {
	const char *ret = setlocale(LC_ALL, "C");
	assert(ret && *ret);

#define TESTCASE(format, scope) { \
	FILE *fp = tmpfile(); \
	assert(fp); \
	assert(fputws((format), fp) >= 0); \
	rewind(fp); \
	scope \
	fclose(fp); } \

	TESTCASE(L"12345", {
		int x = 0;
		fwscanf(fp, L"%d", &x);
		assert(x == 12345);
	})

	TESTCASE(L"z$ 7 5 440", {
		char c;
		int n1;
		int n2;
		int count = fwscanf(fp, L"%*c%c %d %*d %d", &c, &n1, &n2);
		assert(count == 3);
		assert(c == '$');
		assert(n1 == 7);
		assert(n2 == 440);
	})

	TESTCASE(L"process_priority               0\n", {
		char def[80];
		char strparm[128];
		memset(def, '!', 80);
		memset(strparm, '!', 128);
		fwscanf(fp, L"%s %[^\n]\n", def, strparm);
		assert(!strcmp(def, "process_priority"));
		assert(!strcmp(strparm, "0"));
	})

	TESTCASE(L"fffff100", {
		unsigned long y = 0;
		fwscanf(fp, L"%lx", &y);
		assert(y == 0xfffff100);
	})

#if !defined(__i386__) && !defined(__m68k__)
	TESTCASE(L"fffffffff100", {
		unsigned long y = 0;
		fwscanf(fp, L"%lx", &y);
		assert(y == 0xfffffffff100);
	})
#endif

	TESTCASE(L"410dc000", {
		unsigned long y = 0;
		fwscanf(fp, L"%lx", &y);
		assert(y == 0x410dc000);
	})

	TESTCASE(L"MemTotal:       16299664 kB\n", {
		// From webkitgtk
		char token[51] = {0};
		size_t amount = 0;
		int ret = fwscanf(fp, L"%50s%zukB", token, &amount);
		assert(ret == 2);
		assert(!strcmp(token, "MemTotal:"));
		assert(amount == 16299664);
	})

	TESTCASE(L"SIGINT", {
		int sig;
		int ret = fwscanf(fp, L"%d", &sig);
		assert(!ret);
	})

	TESTCASE(L"", {
		wchar_t buf[50];
		int ret = fwscanf(fp, L"%s", buf);
		assert(ret == EOF);
	})

	TESTCASE(L"Managarm", {
		char *str = NULL;
		int ret = fwscanf(fp, L"%ms", &str);
		assert(ret == 1);
		assert(str != NULL);
		assert(!strcmp(str, "Managarm"));
		free(str);
	})

	TESTCASE(L"SomeOption=someValue", {
		// From openjdk
		char name[256];
		char punct;
		int ret = fwscanf(fp, L"%255[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]%c", name, &punct);
		assert(ret == 2);
		assert(punct == '=');
		assert(!strcmp(name, "SomeOption"));
	})

	test_matrix();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
	TESTCASE(L"", assert(fwscanf(fp, L"") == 0);)
#pragma GCC diagnostic pop
	TESTCASE(L"", assert(fwscanf(fp, L"a") == EOF);)
	TESTCASE(L" \t\na", assert(fwscanf(fp, L" a") == 0);)
	TESTCASE(L"a%", assert(fwscanf(fp, L"a%%") == 0);)
	TESTCASE(L"ab", assert(fwscanf(fp, L"abcd") == EOF);)
	TESTCASE(L"abab", assert(fwscanf(fp, L"abcd") == 0);)

	char char_value[16];
	wchar_t wchar_value[16];

	TESTCASE(L"a", { assert(fwscanf(fp, L"%c", char_value) == 1); })
	assert(char_value[0] == 'a');
	TESTCASE(L"a", { assert(fwscanf(fp, L"%lc", wchar_value) == 1); })
	assert(wchar_value[0] == L'a');
	TESTCASE(L"hello", { assert(fwscanf(fp, L"%s", char_value) == 1); })
	assert(strcmp(char_value, "hello") == 0);
	TESTCASE(L"", { assert(fwscanf(fp, L"%s", char_value) == EOF); })
	TESTCASE(L" ", { assert(fwscanf(fp, L"%s", char_value) == EOF); })
	TESTCASE(L" a", { assert(fwscanf(fp, L"%sb", char_value) == 1); })
	TESTCASE(L"hello", { assert(fwscanf(fp, L"%ls", wchar_value) == 1); })
	assert(wcscmp(wchar_value, L"hello") == 0);
	TESTCASE(L"abcd", { assert(fwscanf(fp, L"%[ac]bcd", char_value) == 1); })
	assert(strcmp(char_value, "a") == 0);
	TESTCASE(L"abcd", { assert(fwscanf(fp, L"%l[ac]bcd", wchar_value) == 1); })
	assert(wcscmp(wchar_value, L"a") == 0);
	TESTCASE(L"aacd", { assert(fwscanf(fp, L"%[ac]bcd", char_value) == 1); })
	assert(strcmp(char_value, "aac") == 0);
	TESTCASE(L"aacd", { assert(fwscanf(fp, L"%[ac]bcd", char_value) == 1); })
	assert(strcmp(char_value, "aac") == 0);
	TESTCASE(L"]", { assert(fwscanf(fp, L"%[]]", char_value) == 1); })
	assert(strcmp(char_value, "]") == 0);
	TESTCASE(L"]", { assert(fwscanf(fp, L"%[^]]", char_value) == 0); })
	TESTCASE(L"abcd", { assert(fwscanf(fp, L"%[^ac]bcd", char_value) == 0); })
	TESTCASE(L"aacd", { assert(fwscanf(fp, L"%[^ac]bcd", char_value) == 0); })
	TESTCASE(L"aacd", { assert(fwscanf(fp, L"%[^ac]bcd", char_value) == 0); })
	TESTCASE(L"ddd", { assert(fwscanf(fp, L"%[^ac]bcd", char_value) == 1); })
	assert(strcmp(char_value, "ddd") == 0);
	TESTCASE(L"abcd", { assert(fwscanf(fp, L"%[0-9]", char_value) == 0); })
	TESTCASE(L"2a", { assert(fwscanf(fp, L"%[0-9]", char_value) == 1); })
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

	TESTCASE(L"1234abc", assert(fwscanf(fp, L"%d", &int_value) == 1);)
	assert(int_value == 1234);
	TESTCASE(L"123", assert(fwscanf(fp, L"%hhd", &schar_value) == 1);)
	assert(schar_value == 123);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%hd", &short_value) == 1);)
	assert(short_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%ld", &long_value) == 1);)
	assert(long_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%lld", &longlong_value) == 1);)
	assert(longlong_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%jd", &intmax_value) == 1);)
	assert(intmax_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%zd", &size_value) == 1);)
	assert(size_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%td", &ptrdiff_value) == 1);)
	assert(ptrdiff_value == 1234);
	TESTCASE(L"    1235abc", assert(fwscanf(fp, L"%d", &int_value) == 1);)
	assert(int_value == 1235);
	TESTCASE(L"+1234abc", assert(fwscanf(fp, L"%d", &int_value) == 1);)
	assert(int_value == 1234);
	TESTCASE(L"-1234abc", assert(fwscanf(fp, L"%d", &int_value) == 1);)
	assert(int_value == -1234);
	TESTCASE(L"-abc", assert(fwscanf(fp, L"%d", &int_value) == 0);)
	TESTCASE(L"-", assert(fwscanf(fp, L"%d", &int_value) == 0);)

	TESTCASE(L"1234abc", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 1234);
	TESTCASE(L"123", assert(fwscanf(fp, L"%hhi", &schar_value) == 1);)
	assert(schar_value == 123);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%hi", &short_value) == 1);)
	assert(short_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%li", &long_value) == 1);)
	assert(long_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%lli", &longlong_value) == 1);)
	assert(longlong_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%ji", &intmax_value) == 1);)
	assert(intmax_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%zi", &size_value) == 1);)
	assert(size_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%ti", &ptrdiff_value) == 1);)
	assert(ptrdiff_value == 1234);
	TESTCASE(L"    1235abc", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 1235);
	TESTCASE(L"+1234abc", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 1234);
	TESTCASE(L"-1234abc", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == -1234);
	TESTCASE(L"-abc", assert(fwscanf(fp, L"%i", &int_value) == 0);)

	TESTCASE(L" 0b1011", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0b1011);
	TESTCASE(L" 0B1010", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0b1010);
	TESTCASE(L" +0b1011", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0b1011);
	TESTCASE(L" -0b1111", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == -0b1111);

	TESTCASE(L" 01234", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 01234);
	TESTCASE(L" +01234", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 01234);
	TESTCASE(L" -01234", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == -01234);

	TESTCASE(L" 0xab12", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0xAB12);
	TESTCASE(L" 0xAB12", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0xAB12);
	TESTCASE(L" 0Xab12", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0xAB12);
	TESTCASE(L" +0xab12", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == 0xAB12);
	TESTCASE(L" -0xab12", assert(fwscanf(fp, L"%i", &int_value) == 1);)
	assert(int_value == -0xAB12);

	TESTCASE(L"1234abc", assert(fwscanf(fp, L"%u", &uint_value) == 1);)
	assert(uint_value == 1234);
	TESTCASE(L"123", assert(fwscanf(fp, L"%hhu", &uchar_value) == 1);)
	assert(uchar_value == 123);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%hu", &ushort_value) == 1);)
	assert(ushort_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%lu", &ulong_value) == 1);)
	assert(ulong_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%llu", &ulonglong_value) == 1);)
	assert(ulonglong_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%ju", &uintmax_value) == 1);)
	assert(uintmax_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%zu", &size_value) == 1);)
	assert(size_value == 1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%tu", &ptrdiff_value) == 1);)
	assert(ptrdiff_value == 1234);
	TESTCASE(L"    1235abc", assert(fwscanf(fp, L"%u", &int_value) == 1);)
	assert(int_value == 1235);
	TESTCASE(L"+1234abc", assert(fwscanf(fp, L"%u", &int_value) == 1);)
	assert(int_value == 1234);
	TESTCASE(L"-1234abc", assert(fwscanf(fp, L"%u", &int_value) == 1);)
	assert(int_value == -1234);
	TESTCASE(L"-abc", assert(fwscanf(fp, L"%d", &int_value) == 0);)

	TESTCASE(L" 12345", assert(fwscanf(fp, L"%o", &int_value) == 1);)
	assert(int_value == 012345);
	TESTCASE(L"12", assert(fwscanf(fp, L"%hho", &uchar_value) == 1);)
	assert(uchar_value == 012);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%ho", &ushort_value) == 1);)
	assert(ushort_value == 01234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%lo", &ulong_value) == 1);)
	assert(ulong_value == 01234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%llo", &ulonglong_value) == 1);)
	assert(ulonglong_value == 01234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%jo", &uintmax_value) == 1);)
	assert(uintmax_value == 01234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%zo", &size_value) == 1);)
	assert(size_value == 01234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%to", &ptrdiff_value) == 1);)
	assert(ptrdiff_value == 01234);
	TESTCASE(L" 01234", assert(fwscanf(fp, L"%o", &int_value) == 1);)
	assert(int_value == 01234);
	TESTCASE(L" +01234", assert(fwscanf(fp, L"%o", &int_value) == 1);)
	assert(int_value == 01234);
	TESTCASE(L" -01234", assert(fwscanf(fp, L"%o", &int_value) == 1);)
	assert(int_value == -01234);

	TESTCASE(L" 1234ab", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == 0x1234AB);
	TESTCASE(L"12", assert(fwscanf(fp, L"%hhx", &uchar_value) == 1);)
	assert(uchar_value == 0x12);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%hx", &ushort_value) == 1);)
	assert(ushort_value == 0x1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%lx", &ulong_value) == 1);)
	assert(ulong_value == 0x1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%llx", &ulonglong_value) == 1);)
	assert(ulonglong_value == 0x1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%jx", &uintmax_value) == 1);)
	assert(uintmax_value == 0x1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%zx", &size_value) == 1);)
	assert(size_value == 0x1234);
	TESTCASE(L"1234", assert(fwscanf(fp, L"%tx", &ptrdiff_value) == 1);)
	assert(ptrdiff_value == 0x1234);
	TESTCASE(L" 1234ABC", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == 0x1234ABC);
	TESTCASE(L" 1234ab", assert(fwscanf(fp, L"%X", &int_value) == 1);)
	assert(int_value == 0x1234AB);
	TESTCASE(L" 0x1234", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == 0x1234);
	TESTCASE(L" 0x1234A", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == 0x1234A);
	TESTCASE(L" 0X1234AB", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == 0x1234AB);
	TESTCASE(L" +0x1234", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == 0x1234);
	TESTCASE(L" -0x1234", assert(fwscanf(fp, L"%x", &int_value) == 1);)
	assert(int_value == -0x1234);

	TESTCASE(L"abc", assert(fwscanf(fp, L"abc%n", &int_value) == 0);)
	assert(int_value == 3);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%hhn", &uchar_value) == 0);)
	assert(uchar_value == 4);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%hn", &ushort_value) == 0);)
	assert(ushort_value == 4);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%ln", &ulong_value) == 0);)
	assert(ulong_value == 4);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%lln", &ulonglong_value) == 0);)
	assert(ulonglong_value == 4);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%jn", &uintmax_value) == 0);)
	assert(uintmax_value == 4);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%zn", &size_value) == 0);)
	assert(size_value == 4);
	TESTCASE(L"1234", assert(fwscanf(fp, L"1234%tn", &ptrdiff_value) == 0);)
	assert(ptrdiff_value == 4);
	TESTCASE(L"abcd", assert(fwscanf(fp, L"ab%nc%c", &int_value, char_value) == 1);)
	assert(int_value == 2);
	assert(char_value[0] == 'd');

	float float_value;
	double double_value;
	long double long_double_value;
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%a", &float_value) == 1);)
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%la", &double_value) == 1);)
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%La", &long_double_value) == 1);)
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	TESTCASE(L"1.1234", assert(fwscanf(fp, L"%A", &float_value) == 1);)
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%e", &float_value) == 1);)
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%le", &double_value) == 1);)
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%Le", &long_double_value) == 1);)
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	TESTCASE(L"1.1234", assert(fwscanf(fp, L"%E", &float_value) == 1);)
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%lf", &double_value) == 1);)
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%Lf", &long_double_value) == 1);)
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	TESTCASE(L"1.1234", assert(fwscanf(fp, L"%F", &float_value) == 1);)
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%g", &float_value) == 1);)
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%lg", &double_value) == 1);)
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	TESTCASE(L"1.123", assert(fwscanf(fp, L"%Lg", &long_double_value) == 1);)
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	TESTCASE(L"1.1234", assert(fwscanf(fp, L"%G", &float_value) == 1);)
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	TESTCASE(L"+1.1234", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	TESTCASE(L"-1.1234", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= -1.1234f - 0.01 && float_value <= -1.1234f + 0.01);
	TESTCASE(L"1.1234e3", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 1.1234E3f - 0.01 && float_value <= 1.1234E3f + 0.01);
	TESTCASE(L"1.1234E3", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 1.1234E3f - 0.01 && float_value <= 1.1234E3f + 0.01);
	TESTCASE(L"0x1234", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1234P0f - 0.01 && float_value <= 0x1234P0f + 0.01);
	TESTCASE(L"0X1234", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1234P0f - 0.01 && float_value <= 0x1234P0f + 0.01);
	TESTCASE(L"0x1.1234", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1.1234P0 - 0.01 && float_value <= 0x1.1234P0 + 0.01);
	TESTCASE(L"0x1.abcdP3", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1.ABCDP3 - 0.01 && float_value <= 0x1.ABCDP3 + 0.01);
	TESTCASE(L"0x1.abcdP+3", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1.ABCDP3 - 0.01 && float_value <= 0x1.ABCDP3 + 0.01);
	TESTCASE(L"0x1.81c8P+13", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 12345.0 - 0.01 && float_value <= 12345.0 + 0.01);
	TESTCASE(L"0x1.abcdP-3", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1.ABCDP-3 - 0.01 && float_value <= 0x1.ABCDP-3 + 0.01);
	TESTCASE(L"0x1.AbCdP-3", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1.ABCDP-3 - 0.01 && float_value <= 0x1.ABCDP-3 + 0.01);
	TESTCASE(L"inf", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value));
	TESTCASE(L"InF", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value));
	TESTCASE(L"+inf", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value));
	TESTCASE(L"-inf", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value) && __builtin_signbit(float_value));
	TESTCASE(L"INF", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value));
	TESTCASE(L"infinity", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value));
	TESTCASE(L"INFINITY", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value));
	TESTCASE(L"-infe", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isinf(float_value) && __builtin_signbit(float_value));
	TESTCASE(L"nan", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isnan(float_value));
	TESTCASE(L"NaN", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isnan(float_value));
	TESTCASE(L"naP", assert(fwscanf(fp, L"%f", &float_value) == 0);)
	TESTCASE(L"+nan", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isnan(float_value));
	TESTCASE(L"-nan", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(isnan(float_value));
	TESTCASE(L"-nan", assert(fwscanf(fp, L"%Lf", &long_double_value) == 1);)
	assert(isnan(long_double_value));
	TESTCASE(L"0xhello", assert(fwscanf(fp, L"%f", &float_value) == 0);)
	TESTCASE(L"0x1hello", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 0x1P0f - 0.01 && float_value <= 0x1P0f + 0.01);
	TESTCASE(L"ab1.1234", assert(fwscanf(fp, L"%f", &float_value) == 0);)
	TESTCASE(L"  1.123", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	TESTCASE(L".1", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	assert(float_value >= .1f - 0.01 && float_value <= .1f + 0.01);

	void* ptr;
	TESTCASE(L"hello", assert(fwscanf(fp, L"%p", &ptr) == 0);)
#if UINTPTR_MAX == UINT64_MAX
	TESTCASE(L"0xabcdef12abcdef12", assert(fwscanf(fp, L"%p", &ptr) == 1);)
	assert(ptr == (void*)0xabcdef12abcdef12);
#else
	assert(swscanf(L"0xabcdef12", L"%p", &ptr) == 1);
	assert(ptr == (void*)0xabcdef12);
#endif

	TESTCASE(L"a", assert(fwscanf(fp, L"%*c") == 0);)
	char_value[2] = 'q';
	TESTCASE(L"abcd", assert(fwscanf(fp, L"%2c", char_value) == 1);)
	assert(char_value[2] == 'q');
	char_value[2] = 0;
	assert(strcmp(char_value, "ab") == 0);

	TESTCASE(L"cdef", assert(fwscanf(fp, L"%2s", char_value) == 1);)
	assert(strcmp(char_value, "cd") == 0);
	TESTCASE(L"c def", assert(fwscanf(fp, L"%2s", char_value) == 1);)
	assert(strcmp(char_value, "c") == 0);

	TESTCASE(L"aacd", assert(fwscanf(fp, L"%2[ac]", char_value) == 1);)
	assert(strcmp(char_value, "aa") == 0);

	TESTCASE(L"zz-zxx-mmm", assert(fwscanf(fp, L"%7[zx-]", char_value) == 1);)
	assert(strcmp(char_value, "zz-zxx-") == 0);

	if (!setlocale(LC_ALL, "de_DE.utf8")) {
		puts("setlocale(de_DE.utf8) failed!");
		exit(1);
	}

	TESTCASE(L"12,34", assert(fwscanf(fp, L"%d", &int_value) == 1);)
	assert(int_value == 12);
	TESTCASE(L"12,34", assert(fwscanf(fp, L"%f", &float_value) == 1);)
	// assert(float_value >= 12.34 - 0.01 && float_value <= 12.34 + 0.01);

	TESTCASE(L"hello", {
		int ret = fwscanf(fp, L"%[a-z]", char_value);
		assert(ret == 1);
		assert(!strcmp("hello", char_value));
	});

	TESTCASE(L"hello123", {
		int ret = fwscanf(fp, L"%[^0-9]", char_value);
		assert(ret == 1);
		assert(!strcmp("hello", char_value));
	});

	TESTCASE(L"abcdef", {
		int ret = fwscanf(fp, L"%3[a-z]", char_value);
		assert(ret == 1);
		assert(!strcmp("abc", char_value));
	});

	TESTCASE(L"αβγδ123", {
		int ret = fwscanf(fp, L"%[α-ω]", char_value);
		assert(ret == 1);
		assert(!strcmp("αβγδ", char_value));
		fprintf(stderr, "'%s'\n", char_value);
	});

	TESTCASE(L"αβγδ123", {
		int ret = fwscanf(fp, L"%l[α-ω]", wchar_value);
		assert(ret == 1);
		assert(!wcscmp(L"αβγδ", wchar_value));
		fprintf(stderr, "'%ls'\n", wchar_value);
	});

	TESTCASE(L"αβγδ123", {
		int ret = fwscanf(fp, L"%l[^δ-ω]", wchar_value);
		assert(ret == 1);
		assert(!wcscmp(L"αβγ", wchar_value));
		fprintf(stderr, "'%ls'\n", wchar_value);
	});

	TESTCASE(L"α-β-γ-δ-123", {
		int ret = fwscanf(fp, L"%l[α-ω-]", wchar_value);
		assert(ret == 1);
		assert(!wcscmp(L"α-β-γ-δ-", wchar_value));
		fprintf(stderr, "'%ls'\n", wchar_value);
	});

	TESTCASE(L"α-β-γ-δ-123", {
		int ret = fwscanf(fp, L"%l[^δ-ω-]", wchar_value);
		assert(ret == 1);
		assert(!wcscmp(L"α", wchar_value));
		fprintf(stderr, "'%ls'\n", wchar_value);
	});

	TESTCASE(L"123", {
		int ret = fwscanf(fp, L"%l[a-z]", wchar_value);
		assert(ret == 0);
	});

	return 0;
}

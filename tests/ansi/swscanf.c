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

		switch(f->type) {
			case T_INT: {
				ret = swscanf(f->data, f->format, &data_int);
				assert(data_int == f->expected_int);
				break;
			}
			case T_UINT: {
				ret = swscanf(f->data, f->format, &data_uint);
				assert(data_uint == (unsigned int) f->expected_int);
				break;
			}
			case T_CHAR: {
				ret = swscanf(f->data, f->format, &data_char);
				assert((unsigned char) data_char == (unsigned char) f->expected_int);
				break;
			}
			case T_WCHAR: {
				ret = swscanf(f->data, f->format, &data_wchar);
				assert((wint_t) data_wchar == (wint_t) f->expected_int);
				break;
			}
			case T_NONE: {
				ret = swscanf(f->data, f->format);
				break;
			}

		}

		assert(ret == f->ret);
	}
}

int main() {
	const char *ret = setlocale(LC_ALL, "C");
	assert(ret && *ret);

	{
		int x = 0;
		wchar_t buf[] = L"12345";
		swscanf(buf, L"%d", &x);
		assert(x == 12345);
	}

	{
		char c;
		int n1;
		int n2;
		wchar_t buf[] = L"z$ 7 5 440";;
		int count = swscanf(buf, L"%*c%c %d %*d %d", &c, &n1, &n2);
		assert(count == 3);
		assert(c == '$');
		assert(n1 == 7);
		assert(n2 == 440);
	}

	{
		wchar_t buf[] = L"process_priority               0\n";
		char def[80], strparm[128];
		memset(def, '!', 80);
		memset(strparm, '!', 128);
		swscanf(buf, L"%s %[^\n]\n", def, strparm);
		assert(!strcmp(def, "process_priority"));
		assert(!strcmp(strparm, "0"));
	}

	{
		wchar_t buf[] = L"fffff100";
		unsigned long y = 0;
		swscanf(buf, L"%lx", &y);
		assert(y == 0xfffff100);
	}

#if !defined(__i386__) && !defined(__m68k__)
	{
		wchar_t buf[] = L"fffffffff100";
		unsigned long y = 0;
		swscanf(buf, L"%lx", &y);
		assert(y == 0xfffffffff100);
	}
#endif

	{
		wchar_t buf[] = L"410dc000";
		unsigned long y = 0;
		swscanf(buf, L"%lx", &y);
		assert(y == 0x410dc000);
	}

	{
		// From webkitgtk
		wchar_t buf[] = L"MemTotal:       16299664 kB\n";
		char token[51] = {0};
		size_t amount = 0;
		int ret = swscanf(buf, L"%50s%zukB", token, &amount);
		assert(ret == 2);
		assert(!strcmp(token, "MemTotal:"));
		assert(amount == 16299664);
	}

	{
		wchar_t buf[] = L"SIGINT";
		int sig;
		int ret = swscanf(buf, L"%d", &sig);
		assert(!ret);
	}

	{
		wchar_t buf[50];
		int ret = swscanf(L"", L"%s", buf);
		assert(ret == EOF);
	}

	{
		char *str = NULL;
		int ret = swscanf(L"Managarm", L"%ms", &str);
		assert(ret == 1);
		assert(str != NULL);
		assert(!strcmp(str, "Managarm"));
		free(str);
	}

	{
		// From openjdk
		wchar_t buf[] = L"SomeOption=someValue";
		char name[256];
		char punct;
		int ret = swscanf(buf, L"%255[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]%c", name, &punct);
		assert(ret == 2);
		assert(punct == '=');
		assert(!strcmp(name, "SomeOption"));
	}

	test_matrix();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
	assert(swscanf(L"", L"") == 0);
#pragma GCC diagnostic pop
	assert(swscanf(L"", L"a") == EOF);
	assert(swscanf(L" \t\na", L" a") == 0);
	assert(swscanf(L"a%", L"a%%") == 0);
	assert(swscanf(L"ab", L"abcd") == EOF);
	assert(swscanf(L"abab", L"abcd") == 0);

	char char_value[8];
	wchar_t wchar_value[8];

	assert(swscanf(L"a", L"%c", char_value) == 1);
	assert(char_value[0] == 'a');
	assert(swscanf(L"a", L"%lc", wchar_value) == 1);
	assert(wchar_value[0] == L'a');
	assert(swscanf(L"hello", L"%s", char_value) == 1);
	assert(strcmp(char_value, "hello") == 0);
	assert(swscanf(L"", L"%s", char_value) == EOF);
	assert(swscanf(L" ", L"%s", char_value) == EOF);
	assert(swscanf(L" a", L"%sb", char_value) == 1);
	assert(swscanf(L"hello", L"%ls", wchar_value) == 1);
	assert(wcscmp(wchar_value, L"hello") == 0);
	assert(swscanf(L"abcd", L"%[ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "a") == 0);
	assert(swscanf(L"abcd", L"%l[ac]bcd", wchar_value) == 1);
	assert(wcscmp(wchar_value, L"a") == 0);
	assert(swscanf(L"aacd", L"%[ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "aac") == 0);
	assert(swscanf(L"aacd", L"%[ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "aac") == 0);
	assert(swscanf(L"]", L"%[]]", char_value) == 1);
	assert(strcmp(char_value, "]") == 0);
	assert(swscanf(L"]", L"%[^]]", char_value) == 0);
	assert(swscanf(L"abcd", L"%[^ac]bcd", char_value) == 0);
	assert(swscanf(L"aacd", L"%[^ac]bcd", char_value) == 0);
	assert(swscanf(L"aacd", L"%[^ac]bcd", char_value) == 0);
	assert(swscanf(L"ddd", L"%[^ac]bcd", char_value) == 1);
	assert(strcmp(char_value, "ddd") == 0);
	assert(swscanf(L"abcd", L"%[0-9]", char_value) == 0);
	assert(swscanf(L"2a", L"%[0-9]", char_value) == 1);
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

	assert(swscanf(L"1234abc", L"%d", &int_value) == 1);
	assert(int_value == 1234);
	assert(swscanf(L"123", L"%hhd", &schar_value) == 1);
	assert(schar_value == 123);
	assert(swscanf(L"1234", L"%hd", &short_value) == 1);
	assert(short_value == 1234);
	assert(swscanf(L"1234", L"%ld", &long_value) == 1);
	assert(long_value == 1234);
	assert(swscanf(L"1234", L"%lld", &longlong_value) == 1);
	assert(longlong_value == 1234);
	assert(swscanf(L"1234", L"%jd", &intmax_value) == 1);
	assert(intmax_value == 1234);
	assert(swscanf(L"1234", L"%zd", &size_value) == 1);
	assert(size_value == 1234);
	assert(swscanf(L"1234", L"%td", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 1234);
	assert(swscanf(L"    1235abc", L"%d", &int_value) == 1);
	assert(int_value == 1235);
	assert(swscanf(L"+1234abc", L"%d", &int_value) == 1);
	assert(int_value == 1234);
	assert(swscanf(L"-1234abc", L"%d", &int_value) == 1);
	assert(int_value == -1234);
	assert(swscanf(L"-abc", L"%d", &int_value) == 0);
	assert(swscanf(L"-", L"%d", &int_value) == 0);

	assert(swscanf(L"1234abc", L"%i", &int_value) == 1);
	assert(int_value == 1234);
	assert(swscanf(L"123", L"%hhi", &schar_value) == 1);
	assert(schar_value == 123);
	assert(swscanf(L"1234", L"%hi", &short_value) == 1);
	assert(short_value == 1234);
	assert(swscanf(L"1234", L"%li", &long_value) == 1);
	assert(long_value == 1234);
	assert(swscanf(L"1234", L"%lli", &longlong_value) == 1);
	assert(longlong_value == 1234);
	assert(swscanf(L"1234", L"%ji", &intmax_value) == 1);
	assert(intmax_value == 1234);
	assert(swscanf(L"1234", L"%zi", &size_value) == 1);
	assert(size_value == 1234);
	assert(swscanf(L"1234", L"%ti", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 1234);
	assert(swscanf(L"    1235abc", L"%i", &int_value) == 1);
	assert(int_value == 1235);
	assert(swscanf(L"+1234abc", L"%i", &int_value) == 1);
	assert(int_value == 1234);
	assert(swscanf(L"-1234abc", L"%i", &int_value) == 1);
	assert(int_value == -1234);
	assert(swscanf(L"-abc", L"%i", &int_value) == 0);

	assert(swscanf(L" 0b1011", L"%i", &int_value) == 1);
	assert(int_value == 0b1011);
	assert(swscanf(L" 0B1010", L"%i", &int_value) == 1);
	assert(int_value == 0b1010);
	assert(swscanf(L" +0b1011", L"%i", &int_value) == 1);
	assert(int_value == 0b1011);
	assert(swscanf(L" -0b1111", L"%i", &int_value) == 1);
	assert(int_value == -0b1111);

	assert(swscanf(L" 01234", L"%i", &int_value) == 1);
	assert(int_value == 01234);
	assert(swscanf(L" +01234", L"%i", &int_value) == 1);
	assert(int_value == 01234);
	assert(swscanf(L" -01234", L"%i", &int_value) == 1);
	assert(int_value == -01234);

	assert(swscanf(L" 0xab12", L"%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(swscanf(L" 0xAB12", L"%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(swscanf(L" 0Xab12", L"%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(swscanf(L" +0xab12", L"%i", &int_value) == 1);
	assert(int_value == 0xAB12);
	assert(swscanf(L" -0xab12", L"%i", &int_value) == 1);
	assert(int_value == -0xAB12);

	assert(swscanf(L"1234abc", L"%u", &uint_value) == 1);
	assert(uint_value == 1234);
	assert(swscanf(L"123", L"%hhu", &uchar_value) == 1);
	assert(uchar_value == 123);
	assert(swscanf(L"1234", L"%hu", &ushort_value) == 1);
	assert(ushort_value == 1234);
	assert(swscanf(L"1234", L"%lu", &ulong_value) == 1);
	assert(ulong_value == 1234);
	assert(swscanf(L"1234", L"%llu", &ulonglong_value) == 1);
	assert(ulonglong_value == 1234);
	assert(swscanf(L"1234", L"%ju", &uintmax_value) == 1);
	assert(uintmax_value == 1234);
	assert(swscanf(L"1234", L"%zu", &size_value) == 1);
	assert(size_value == 1234);
	assert(swscanf(L"1234", L"%tu", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 1234);
	assert(swscanf(L"    1235abc", L"%u", &int_value) == 1);
	assert(int_value == 1235);
	assert(swscanf(L"+1234abc", L"%u", &int_value) == 1);
	assert(int_value == 1234);
	assert(swscanf(L"-1234abc", L"%u", &int_value) == 1);
	assert(int_value == -1234);
	assert(swscanf(L"-abc", L"%d", &int_value) == 0);

	assert(swscanf(L" 12345", L"%o", &int_value) == 1);
	assert(int_value == 012345);
	assert(swscanf(L"12", L"%hho", &uchar_value) == 1);
	assert(uchar_value == 012);
	assert(swscanf(L"1234", L"%ho", &ushort_value) == 1);
	assert(ushort_value == 01234);
	assert(swscanf(L"1234", L"%lo", &ulong_value) == 1);
	assert(ulong_value == 01234);
	assert(swscanf(L"1234", L"%llo", &ulonglong_value) == 1);
	assert(ulonglong_value == 01234);
	assert(swscanf(L"1234", L"%jo", &uintmax_value) == 1);
	assert(uintmax_value == 01234);
	assert(swscanf(L"1234", L"%zo", &size_value) == 1);
	assert(size_value == 01234);
	assert(swscanf(L"1234", L"%to", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 01234);
	assert(swscanf(L" 01234", L"%o", &int_value) == 1);
	assert(int_value == 01234);
	assert(swscanf(L" +01234", L"%o", &int_value) == 1);
	assert(int_value == 01234);
	assert(swscanf(L" -01234", L"%o", &int_value) == 1);
	assert(int_value == -01234);

	assert(swscanf(L" 1234ab", L"%x", &int_value) == 1);
	assert(int_value == 0x1234AB);
	assert(swscanf(L"12", L"%hhx", &uchar_value) == 1);
	assert(uchar_value == 0x12);
	assert(swscanf(L"1234", L"%hx", &ushort_value) == 1);
	assert(ushort_value == 0x1234);
	assert(swscanf(L"1234", L"%lx", &ulong_value) == 1);
	assert(ulong_value == 0x1234);
	assert(swscanf(L"1234", L"%llx", &ulonglong_value) == 1);
	assert(ulonglong_value == 0x1234);
	assert(swscanf(L"1234", L"%jx", &uintmax_value) == 1);
	assert(uintmax_value == 0x1234);
	assert(swscanf(L"1234", L"%zx", &size_value) == 1);
	assert(size_value == 0x1234);
	assert(swscanf(L"1234", L"%tx", &ptrdiff_value) == 1);
	assert(ptrdiff_value == 0x1234);
	assert(swscanf(L" 1234ABC", L"%x", &int_value) == 1);
	assert(int_value == 0x1234ABC);
	assert(swscanf(L" 1234ab", L"%X", &int_value) == 1);
	assert(int_value == 0x1234AB);
	assert(swscanf(L" 0x1234", L"%x", &int_value) == 1);
	assert(int_value == 0x1234);
	assert(swscanf(L" 0x1234A", L"%x", &int_value) == 1);
	assert(int_value == 0x1234A);
	assert(swscanf(L" 0X1234AB", L"%x", &int_value) == 1);
	assert(int_value == 0x1234AB);
	assert(swscanf(L" +0x1234", L"%x", &int_value) == 1);
	assert(int_value == 0x1234);
	assert(swscanf(L" -0x1234", L"%x", &int_value) == 1);
	assert(int_value == -0x1234);

	assert(swscanf(L"abc", L"abc%n", &int_value) == 0);
	assert(int_value == 3);
	assert(swscanf(L"1234", L"1234%hhn", &uchar_value) == 0);
	assert(uchar_value == 4);
	assert(swscanf(L"1234", L"1234%hn", &ushort_value) == 0);
	assert(ushort_value == 4);
	assert(swscanf(L"1234", L"1234%ln", &ulong_value) == 0);
	assert(ulong_value == 4);
	assert(swscanf(L"1234", L"1234%lln", &ulonglong_value) == 0);
	assert(ulonglong_value == 4);
	assert(swscanf(L"1234", L"1234%jn", &uintmax_value) == 0);
	assert(uintmax_value == 4);
	assert(swscanf(L"1234", L"1234%zn", &size_value) == 0);
	assert(size_value == 4);
	assert(swscanf(L"1234", L"1234%tn", &ptrdiff_value) == 0);
	assert(ptrdiff_value == 4);
	assert(swscanf(L"abcd", L"ab%nc%c", &int_value, char_value) == 1);
	assert(int_value == 2);
	assert(char_value[0] == 'd');

	float float_value;
	double double_value;
	long double long_double_value;
	assert(swscanf(L"1.123", L"%a", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(swscanf(L"1.123", L"%la", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(swscanf(L"1.123", L"%La", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(swscanf(L"1.1234", L"%A", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(swscanf(L"1.123", L"%e", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(swscanf(L"1.123", L"%le", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(swscanf(L"1.123", L"%Le", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(swscanf(L"1.1234", L"%E", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(swscanf(L"1.123", L"%f", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(swscanf(L"1.123", L"%lf", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(swscanf(L"1.123", L"%Lf", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(swscanf(L"1.1234", L"%F", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(swscanf(L"1.123", L"%g", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(swscanf(L"1.123", L"%lg", &double_value) == 1);
	assert(double_value >= 1.123 - 0.01 && double_value <= 1.123 + 0.01);
	assert(swscanf(L"1.123", L"%Lg", &long_double_value) == 1);
	assert(long_double_value >= 1.123L - 0.01 && long_double_value <= 1.123L + 0.01);
	assert(swscanf(L"1.1234", L"%G", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(swscanf(L"+1.1234", L"%f", &float_value) == 1);
	assert(float_value >= 1.1234f - 0.01 && float_value <= 1.1234f + 0.01);
	assert(swscanf(L"-1.1234", L"%f", &float_value) == 1);
	assert(float_value >= -1.1234f - 0.01 && float_value <= -1.1234f + 0.01);
	assert(swscanf(L"1.1234e3", L"%f", &float_value) == 1);
	assert(float_value >= 1.1234E3f - 0.01 && float_value <= 1.1234E3f + 0.01);
	assert(swscanf(L"1.1234E3", L"%f", &float_value) == 1);
	assert(float_value >= 1.1234E3f - 0.01 && float_value <= 1.1234E3f + 0.01);
	assert(swscanf(L"0x1234", L"%f", &float_value) == 1);
	assert(float_value >= 0x1234P0f - 0.01 && float_value <= 0x1234P0f + 0.01);
	assert(swscanf(L"0X1234", L"%f", &float_value) == 1);
	assert(float_value >= 0x1234P0f - 0.01 && float_value <= 0x1234P0f + 0.01);
	assert(swscanf(L"0x1.1234", L"%f", &float_value) == 1);
	assert(float_value >= 0x1.1234P0 - 0.01 && float_value <= 0x1.1234P0 + 0.01);
	assert(swscanf(L"0x1.abcdP3", L"%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP3 - 0.01 && float_value <= 0x1.ABCDP3 + 0.01);
	assert(swscanf(L"0x1.abcdP+3", L"%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP3 - 0.01 && float_value <= 0x1.ABCDP3 + 0.01);
	assert(swscanf(L"0x1.81c8P+13", L"%f", &float_value) == 1);
	assert(float_value >= 12345.0 - 0.01 && float_value <= 12345.0 + 0.01);
	assert(swscanf(L"0x1.abcdP-3", L"%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP-3 - 0.01 && float_value <= 0x1.ABCDP-3 + 0.01);
	assert(swscanf(L"0x1.AbCdP-3", L"%f", &float_value) == 1);
	assert(float_value >= 0x1.ABCDP-3 - 0.01 && float_value <= 0x1.ABCDP-3 + 0.01);
	assert(swscanf(L"inf", L"%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(swscanf(L"InF", L"%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(swscanf(L"+inf", L"%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(swscanf(L"-inf", L"%f", &float_value) == 1);
	assert(isinf(float_value) && __builtin_signbit(float_value));
	assert(swscanf(L"INF", L"%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(swscanf(L"infinity", L"%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(swscanf(L"INFINITY", L"%f", &float_value) == 1);
	assert(isinf(float_value));
	assert(swscanf(L"-infe", L"%f", &float_value) == 1);
	assert(isinf(float_value) && __builtin_signbit(float_value));
	assert(swscanf(L"nan", L"%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(swscanf(L"NaN", L"%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(swscanf(L"naP", L"%f", &float_value) == 0);
	assert(swscanf(L"+nan", L"%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(swscanf(L"-nan", L"%f", &float_value) == 1);
	assert(isnan(float_value));
	assert(swscanf(L"-nan", L"%Lf", &long_double_value) == 1);
	assert(isnan(long_double_value));
	assert(swscanf(L"0xhello", L"%f", &float_value) == 0);
	assert(swscanf(L"0x1hello", L"%f", &float_value) == 1);
	assert(float_value >= 0x1P0f - 0.01 && float_value <= 0x1P0f + 0.01);
	assert(swscanf(L"ab1.1234", L"%f", &float_value) == 0);
	assert(swscanf(L"  1.123", L"%f", &float_value) == 1);
	assert(float_value >= 1.123f - 0.01 && float_value <= 1.123f + 0.01);
	assert(swscanf(L".1", L"%f", &float_value) == 1);
	assert(float_value >= .1f - 0.01 && float_value <= .1f + 0.01);

	void* ptr;
	assert(swscanf(L"hello", L"%p", &ptr) == 0);
#if UINTPTR_MAX == UINT64_MAX
	assert(swscanf(L"0xabcdef12abcdef12", L"%p", &ptr) == 1);
	assert(ptr == (void*)0xabcdef12abcdef12);
#else
	assert(swscanf(L"0xabcdef12", L"%p", &ptr) == 1);
	assert(ptr == (void*)0xabcdef12);
#endif

	assert(swscanf(L"a", L"%*c") == 0);
	char_value[2] = 'q';
	assert(swscanf(L"abcd", L"%2c", char_value) == 1);
	assert(char_value[2] == 'q');
	char_value[2] = 0;
	assert(strcmp(char_value, "ab") == 0);

	assert(swscanf(L"cdef", L"%2s", char_value) == 1);
	assert(strcmp(char_value, "cd") == 0);
	assert(swscanf(L"c def", L"%2s", char_value) == 1);
	assert(strcmp(char_value, "c") == 0);

	assert(swscanf(L"aacd", L"%2[ac]", char_value) == 1);
	assert(strcmp(char_value, "aa") == 0);

	assert(swscanf(L"zz-zxx-mmm", L"%7[zx-]", char_value) == 1);
	assert(strcmp(char_value, "zz-zxx-") == 0);

	if (!setlocale(LC_ALL, "de_DE.utf8")) {
		puts("setlocale(de_DE.utf8) failed!");
		exit(1);
	}

	assert(swscanf(L"12,34", L"%d", &int_value) == 1);
	assert(int_value == 12);
	assert(swscanf(L"12,34", L"%f", &float_value) == 1);
	// assert(float_value >= 12.34 - 0.01 && float_value <= 12.34 + 0.01);

	return 0;
}

#include <assert.h>
#include <errno.h>
#include <fenv.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define BUFFER_SIZE 1024

#define test_roundtrip(val, print_format, scanf_format, expected_string, expected_scanf_ret) { \
	double d = val; \
	swprintf(buffer, BUFFER_SIZE, print_format, val); \
	if (wcscmp(buffer, expected_string)) fprintf(stderr, "'%ls' != '%ls'\n", buffer, expected_string); \
	assert(!wcscmp(buffer, expected_string)); \
	assert(swscanf(buffer, scanf_format, &d) == expected_scanf_ret); \
	assert(d == val); \
}

int main() {
	const char *ret = setlocale(LC_ALL, "C.utf8");
	assert(ret && *ret);

	wchar_t buffer[BUFFER_SIZE];
	int written_len;

	written_len = swprintf(buffer, BUFFER_SIZE, L"integer: %d | hex: 0x%04X", 42, 255);
	assert(written_len == 25);
	assert(!wcscmp(L"integer: 42 | hex: 0x00FF", buffer));

	written_len = swprintf(buffer, BUFFER_SIZE, L"Value: |%-10.2f|", 3.14159);
	assert(written_len == 19);

	wchar_t star = L'★';
	wchar_t *greeting = L"こんにちは";

	written_len = swprintf(buffer, BUFFER_SIZE, L"Symbol: %lc | Text: %ls", star, greeting);
	assert(written_len == 23);

	written_len = swprintf(buffer, 10, L"This string is definitely too long");
	assert(written_len < 0);
	assert(errno == E2BIG);

	written_len = swprintf(buffer, 10, L"abcdefghi");
	assert(written_len == 9);

	written_len = swprintf(buffer, 10, L"abcdefghij");
	assert(written_len < 0);
	assert(errno == E2BIG);

	written_len = swprintf(buffer, 10, L"%s", "€öß");
	assert(written_len == 3);

	// tests ported from ansi/sprintf
	ret = setlocale(LC_ALL, "C");
	assert(ret && *ret);

	swprintf(buffer, BUFFER_SIZE, L"%d", 12);
	assert(!wcscmp(buffer, L"12"));

	swprintf(buffer, BUFFER_SIZE, L"%f", 3.14);
	assert(!wcscmp(buffer, L"3.140000"));
	swprintf(buffer, BUFFER_SIZE, L"%.1f", 0.0);
	assert(!wcscmp(buffer, L"0.0"));

	// Test %g
	swprintf(buffer, BUFFER_SIZE, L"%g", 0.0);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%.2g", 0.01234);
	assert(!wcscmp(buffer, L"0.012"));
	swprintf(buffer, BUFFER_SIZE, L"%.3g", 1.1);
	assert(!wcscmp(buffer, L"1.1"));
	swprintf(buffer, BUFFER_SIZE, L"%#.5g", 1.1);
	assert(!wcscmp(buffer, L"1.1000"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 0.0000123);
	assert(!wcscmp(buffer, L"1.23e-05"));

	// Test %e
	swprintf(buffer, BUFFER_SIZE, L"%4.3e", 0.0);
	assert(!wcscmp(buffer, L"0.000e+00"));
	swprintf(buffer, BUFFER_SIZE, L"%.3e", 6.9e27);
	assert(!wcscmp(buffer, L"6.900e+27"));

	// Test %c right padding.
	swprintf(buffer, BUFFER_SIZE, L"%-2c", 'a');
	assert(!wcscmp(buffer, L"a "));

	// Test %c left padding.
	swprintf(buffer, BUFFER_SIZE, L"%2c", 'a');
	assert(!wcscmp(buffer, L" a"));

	// Test %d right padding - mlibc issue #58.
	swprintf(buffer, BUFFER_SIZE, L"%-2d", 1);
	assert(!wcscmp(buffer, L"1 "));
	swprintf(buffer, BUFFER_SIZE, L"%-2.2d", 1);
	assert(!wcscmp(buffer, L"01"));
	swprintf(buffer, BUFFER_SIZE, L"%-3.2d", 1);
	assert(!wcscmp(buffer, L"01 "));
	swprintf(buffer, BUFFER_SIZE, L"%-3.2d", 12);
	assert(!wcscmp(buffer, L"12 "));
	swprintf(buffer, BUFFER_SIZE, L"%-3.2d", 123);
	assert(!wcscmp(buffer, L"123"));
	swprintf(buffer, BUFFER_SIZE, L"%-3.2u", 12);
	assert(!wcscmp(buffer, L"12 "));

	// Test %d left padding.
	swprintf(buffer, BUFFER_SIZE, L"%2d", 1);
	assert(!wcscmp(buffer, L" 1"));
	swprintf(buffer, BUFFER_SIZE, L"%3.2d", 1);
	assert(!wcscmp(buffer, L" 01"));
	swprintf(buffer, BUFFER_SIZE, L"%3.2d", 12);
	assert(!wcscmp(buffer, L" 12"));
	swprintf(buffer, BUFFER_SIZE, L"%3.2d", 123);
	assert(!wcscmp(buffer, L"123"));
	swprintf(buffer, BUFFER_SIZE, L"%3.2u", 12);
	assert(!wcscmp(buffer, L" 12"));
	// Disable -Wformat here since we deliberately induce some warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	swprintf(buffer, BUFFER_SIZE, L"%04.3u", 12);
	assert(!wcscmp(buffer, L" 012"));
#pragma GCC diagnostic pop
	swprintf(buffer, BUFFER_SIZE, L"%+04d", 0);
	assert(!wcscmp(buffer, L"+000"));

	// Test %f padding.
	// TODO: Test printing of huge numbers (larger than 2^64)
	swprintf(buffer, BUFFER_SIZE, L"%2.f", 1.2);
	assert(!wcscmp(buffer, L" 1"));
	swprintf(buffer, BUFFER_SIZE, L"%2.f", 12.3);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%5.2f", 1.0);
	assert(!wcscmp(buffer, L" 1.00"));
	swprintf(buffer, BUFFER_SIZE, L"%.1f", -4.0);
	assert(!wcscmp(buffer, L"-4.0"));
	swprintf(buffer, BUFFER_SIZE, L"%-3.f", 8.0);
	assert(!wcscmp(buffer, L"8  "));
	swprintf(buffer, BUFFER_SIZE, L"%4f", INFINITY);
	assert(!wcscmp(buffer, L" inf") || !wcscmp(buffer, L"infinity"));
	swprintf(buffer, BUFFER_SIZE, L"%4f", NAN);
	assert(!wcscmp(buffer, L" nan"));
	swprintf(buffer, BUFFER_SIZE, L"%4F", INFINITY);
	assert(!wcscmp(buffer, L" INF") || !wcscmp(buffer, L"INFINITY"));
	swprintf(buffer, BUFFER_SIZE, L"%4F", NAN);
	assert(!wcscmp(buffer, L" NAN"));
	swprintf(buffer, BUFFER_SIZE, L"%05.2f", 1.0);
	assert(!wcscmp(buffer, L"01.00"));
	swprintf(buffer, BUFFER_SIZE, L"%09f", INFINITY); // 0 ignored when padding infs
	assert(!wcscmp(buffer, L"      inf") || !wcscmp(buffer, L" infinity"));

	// Test %f rounding
	swprintf(buffer, BUFFER_SIZE, L"%5.2f", 1.2);
	assert(!wcscmp(buffer, L" 1.20"));
	swprintf(buffer, BUFFER_SIZE, L"%5.2f", 1.23);
	assert(!wcscmp(buffer, L" 1.23"));
	swprintf(buffer, BUFFER_SIZE, L"%5.2f", 1.234);
	assert(!wcscmp(buffer, L" 1.23"));
	swprintf(buffer, BUFFER_SIZE, L"%5.2f", 12.345);
	assert(!wcscmp(buffer, L"12.35"));
	swprintf(buffer, BUFFER_SIZE, L"%-5.2f", 1.2);
	assert(!wcscmp(buffer, L"1.20 "));

	// Test '+' and ' ' flags - mlibc issue #229.
	// Disable -Wformat here since we deliberately induce some warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	swprintf(buffer, BUFFER_SIZE, L"%+d", 12);
	assert(!wcscmp(buffer, L"+12"));
	swprintf(buffer, BUFFER_SIZE, L"% d", 12);
	assert(!wcscmp(buffer, L" 12"));
	swprintf(buffer, BUFFER_SIZE, L"% +d", 12);
	assert(!wcscmp(buffer, L"+12"));
	swprintf(buffer, BUFFER_SIZE, L"%+ d", 12);
	assert(!wcscmp(buffer, L"+12"));
	swprintf(buffer, BUFFER_SIZE, L"%+d", -12);
	assert(!wcscmp(buffer, L"-12"));
	swprintf(buffer, BUFFER_SIZE, L"% d", -12);
	assert(!wcscmp(buffer, L"-12"));
	swprintf(buffer, BUFFER_SIZE, L"% +d", -12);
	assert(!wcscmp(buffer, L"-12"));
	swprintf(buffer, BUFFER_SIZE, L"%+ d", -12);
	assert(!wcscmp(buffer, L"-12"));
#pragma GCC diagnostic pop

	// Test '#' flag.
	// TODO: Test with a, A, e, E, f, F, g, G conversions.
	swprintf(buffer, BUFFER_SIZE, L"%#x", 12);
	assert(!wcscmp(buffer, L"0xc"));
	swprintf(buffer, BUFFER_SIZE, L"%#X", 12);
	assert(!wcscmp(buffer, L"0XC"));
	swprintf(buffer, BUFFER_SIZE, L"%#o", 12);
	assert(!wcscmp(buffer, L"014"));

	swprintf(buffer, BUFFER_SIZE, L"%#x", 0);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%#X", 0);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%#o", 0);
	assert(!wcscmp(buffer, L"0"));

	// Disable -Wformat here because the compiler might not know about the b specifier.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	swprintf(buffer, BUFFER_SIZE, L"%#b", 12);
	assert(!wcscmp(buffer, L"0b1100"));
	swprintf(buffer, BUFFER_SIZE, L"%#B", 12);
	assert(!wcscmp(buffer, L"0B1100"));
	swprintf(buffer, BUFFER_SIZE, L"%#b", 0);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%#B", 0);
	assert(!wcscmp(buffer, L"0"));
#pragma GCC diagnostic pop

	// Test 'd' with different size mods to see
	// if they work
	swprintf(buffer, BUFFER_SIZE, L"%d", 12);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%ld", 12L);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%lld", 12LL);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%zd", (size_t)12);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%hd", (short) 12);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%hhd", (char) 12);
	assert(!wcscmp(buffer, L"12"));

	// Test 'x' with different size mods to see
	// if they work
	swprintf(buffer, BUFFER_SIZE, L"%x", 12);
	assert(!wcscmp(buffer, L"c"));
	swprintf(buffer, BUFFER_SIZE, L"%lx", 12L);
	assert(!wcscmp(buffer, L"c"));
	swprintf(buffer, BUFFER_SIZE, L"%llx", 12LL);
	assert(!wcscmp(buffer, L"c"));
	swprintf(buffer, BUFFER_SIZE, L"%zx", (size_t)12);
	assert(!wcscmp(buffer, L"c"));
	swprintf(buffer, BUFFER_SIZE, L"%hx", (unsigned short) 12);
	assert(!wcscmp(buffer, L"c"));
	swprintf(buffer, BUFFER_SIZE, L"%hhx", (unsigned char) 12);
	assert(!wcscmp(buffer, L"c"));

	// Test 'X' with different size mods to see
	// if they work
	swprintf(buffer, BUFFER_SIZE, L"%X", 12);
	assert(!wcscmp(buffer, L"C"));
	swprintf(buffer, BUFFER_SIZE, L"%lX", 12L);
	assert(!wcscmp(buffer, L"C"));
	swprintf(buffer, BUFFER_SIZE, L"%llX", 12LL);
	assert(!wcscmp(buffer, L"C"));
	swprintf(buffer, BUFFER_SIZE, L"%zX", (size_t)12);
	assert(!wcscmp(buffer, L"C"));
	swprintf(buffer, BUFFER_SIZE, L"%hX", (unsigned short) 12);
	assert(!wcscmp(buffer, L"C"));
	swprintf(buffer, BUFFER_SIZE, L"%hhX", (unsigned char) 12);
	assert(!wcscmp(buffer, L"C"));

	// Test 'o' with different size mods to see
	// if they work
	swprintf(buffer, BUFFER_SIZE, L"%o", 12);
	assert(!wcscmp(buffer, L"14"));
	swprintf(buffer, BUFFER_SIZE, L"%lo", 12L);
	assert(!wcscmp(buffer, L"14"));
	swprintf(buffer, BUFFER_SIZE, L"%llo", 12LL);
	assert(!wcscmp(buffer, L"14"));
	swprintf(buffer, BUFFER_SIZE, L"%zo", (size_t)12);
	assert(!wcscmp(buffer, L"14"));
	swprintf(buffer, BUFFER_SIZE, L"%ho", (unsigned short) 12);
	assert(!wcscmp(buffer, L"14"));
	swprintf(buffer, BUFFER_SIZE, L"%hho", (unsigned char) 12);
	assert(!wcscmp(buffer, L"14"));

	// Disable -Wformat here because the compiler might not know about the b specifier.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	// Test 'b' with different size mods to see
	// if they work
	swprintf(buffer, BUFFER_SIZE, L"%b", 12);
	assert(!wcscmp(buffer, L"1100"));
	swprintf(buffer, BUFFER_SIZE, L"%lb", 12L);
	assert(!wcscmp(buffer, L"1100"));
	swprintf(buffer, BUFFER_SIZE, L"%llb", 12LL);
	assert(!wcscmp(buffer, L"1100"));
	swprintf(buffer, BUFFER_SIZE, L"%zb", (size_t)12);
	assert(!wcscmp(buffer, L"1100"));
	swprintf(buffer, BUFFER_SIZE, L"%hb", (unsigned short) 12);
	assert(!wcscmp(buffer, L"1100"));
	swprintf(buffer, BUFFER_SIZE, L"%hhb", (unsigned char) 12);
	assert(!wcscmp(buffer, L"1100"));
#pragma GCC diagnostic pop

	// Test %n$ syntax.
	swprintf(buffer, BUFFER_SIZE, L"%1$d", 12);
	assert(!wcscmp(buffer, L"12"));
	swprintf(buffer, BUFFER_SIZE, L"%1$d %1$d", 12);
	assert(!wcscmp(buffer, L"12 12"));
	swprintf(buffer, BUFFER_SIZE, L"%1$d %2$d %1$d", 12, 14);
	assert(!wcscmp(buffer, L"12 14 12"));
	swprintf(buffer, BUFFER_SIZE, L"%1$d %2$s %2$s", 12, "foo");
	assert(!wcscmp(buffer, L"12 foo foo"));

	swprintf(buffer, BUFFER_SIZE, L"%1$s%5$s%6$s%3$s%4$s%2$s", "a", "b", "u", "n", "h", "N");
	assert(!wcscmp(buffer, L"ahNunb"));

	// test negative precision
	swprintf(buffer, BUFFER_SIZE, L"%.*g", -2, 15.1234567);
	assert(!wcscmp(buffer, L"15.1235"));
	swprintf(buffer, BUFFER_SIZE, L"%*g", -5, 15.1);
	assert(!wcscmp(buffer, L"15.1 "));
	// Disable -Wformat as the compiler rightfully warns that the '0' flag is ignored
	// when the '0' flag is used
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	swprintf(buffer, BUFFER_SIZE, L"%0-6g", 69.2);
	assert(!wcscmp(buffer, L"69.2  "));
#pragma GCC diagnostic pop

	// More extensive floating point tests.
	// Test zero and negative zero
	swprintf(buffer, BUFFER_SIZE, L"%f", 0.0);
	assert(!wcscmp(buffer, L"0.000000"));
	swprintf(buffer, BUFFER_SIZE, L"%f", -0.0);
	assert(!wcscmp(buffer, L"-0.000000"));
	swprintf(buffer, BUFFER_SIZE, L"%e", 0.0);
	assert(!wcscmp(buffer, L"0.000000e+00"));
	swprintf(buffer, BUFFER_SIZE, L"%e", -0.0);
	assert(!wcscmp(buffer, L"-0.000000e+00"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 0.0);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%g", -0.0);
	assert(!wcscmp(buffer, L"-0"));

	// Test rounding
	swprintf(buffer, BUFFER_SIZE, L"%.2f", 1.235);
	assert(!wcscmp(buffer, L"1.24"));
	swprintf(buffer, BUFFER_SIZE, L"%.2f", -1.235);
	assert(!wcscmp(buffer, L"-1.24"));
	// 1.2349999999999998 is a double literal that is slightly less than 1.235
	swprintf(buffer, BUFFER_SIZE, L"%.2f", 1.2349999999999998);
	assert(!wcscmp(buffer, L"1.23"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", 0.49);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", -0.49);
	assert(!wcscmp(buffer, L"-0"));

	// Test round-to-even behavior
	fesetround(FE_TONEAREST);

	swprintf(buffer, BUFFER_SIZE, L"%.0f", 0.5);
	assert(!wcscmp(buffer, L"0"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", -0.5);
	assert(!wcscmp(buffer, L"-0"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", 0.51);
	assert(!wcscmp(buffer, L"1"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", -0.51);
	assert(!wcscmp(buffer, L"-1"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", 2.5);
	assert(!wcscmp(buffer, L"2"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", -2.5);
	assert(!wcscmp(buffer, L"-2"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", 2.51);
	assert(!wcscmp(buffer, L"3"));
	swprintf(buffer, BUFFER_SIZE, L"%.0f", -2.51);
	assert(!wcscmp(buffer, L"-3"));

	// Test %e/%E
	swprintf(buffer, BUFFER_SIZE, L"%.3e", 12345.0);
	assert(!wcscmp(buffer, L"1.234e+04"));
	swprintf(buffer, BUFFER_SIZE, L"%.3E", 12345.0);
	assert(!wcscmp(buffer, L"1.234E+04"));
	swprintf(buffer, BUFFER_SIZE, L"%.3e", 0.00012345);
	assert(!wcscmp(buffer, L"1.234e-04"));
	swprintf(buffer, BUFFER_SIZE, L"%.3E", -0.00012345);
	assert(!wcscmp(buffer, L"-1.234E-04"));

	// Test %g/%G behavior
	swprintf(buffer, BUFFER_SIZE, L"%g", 123456.0); // Should not use e-notation
	assert(!wcscmp(buffer, L"123456"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 1234567.0); // Should use e-notation
	assert(!wcscmp(buffer, L"1.23457e+06"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 12345.0);
	assert(!wcscmp(buffer, L"12345"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 1.2345);
	assert(!wcscmp(buffer, L"1.2345"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 0.00012345); // Should not use e-notation
	assert(!wcscmp(buffer, L"0.00012345"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 0.000012345); // Should use e-notation
	assert(!wcscmp(buffer, L"1.2345e-05"));
	swprintf(buffer, BUFFER_SIZE, L"%12g", -1.2345);
	assert(!wcscmp(buffer, L"     -1.2345"));

	// %g precision
	swprintf(buffer, BUFFER_SIZE, L"%.3g", 12345.0);
	assert(!wcscmp(buffer, L"1.23e+04"));
	swprintf(buffer, BUFFER_SIZE, L"%.3G", 12345.0);
	assert(!wcscmp(buffer, L"1.23E+04"));
	swprintf(buffer, BUFFER_SIZE, L"%.3g", 1.234);
	assert(!wcscmp(buffer, L"1.23"));
	swprintf(buffer, BUFFER_SIZE, L"%.3g", 0.001234);
	assert(!wcscmp(buffer, L"0.00123"));
	swprintf(buffer, BUFFER_SIZE, L"%.3g", 0.0001234); // Should not use e-notation
	assert(!wcscmp(buffer, L"0.000123"));
	swprintf(buffer, BUFFER_SIZE, L"%.3g", 0.00001234); // Should use e-notation
	assert(!wcscmp(buffer, L"1.23e-05"));
	swprintf(buffer, BUFFER_SIZE, L"%08.1g", 69.2);
	assert(!wcscmp(buffer, L"0007e+01"));
	swprintf(buffer, BUFFER_SIZE, L"%#08.4g", 69.29854);
	assert(!wcscmp(buffer, L"00069.30"));

	// %g trailing zeros
	swprintf(buffer, BUFFER_SIZE, L"%g", 1.200);
	assert(!wcscmp(buffer, L"1.2"));
	swprintf(buffer, BUFFER_SIZE, L"%#g", 1.200);
	assert(!wcscmp(buffer, L"1.20000"));
	swprintf(buffer, BUFFER_SIZE, L"%g", 1200.0);
	assert(!wcscmp(buffer, L"1200"));
	swprintf(buffer, BUFFER_SIZE, L"%#g", 1200.0);
	assert(!wcscmp(buffer, L"1200.00"));

	// %a + %A
	test_roundtrip(3.14, L"%a", L"%lf", L"0x1.91eb851eb851fp+1", 1);
	test_roundtrip(3.14, L"%A", L"%lf", L"0X1.91EB851EB851FP+1", 1);
	test_roundtrip(12345.0, L"%a", L"%lf", L"0x1.81c8p+13", 1);
	test_roundtrip(12345.0, L"%A", L"%lf", L"0X1.81C8P+13", 1);

	swprintf(buffer, BUFFER_SIZE, L"%.3a", 12345.0);
	assert(!wcscmp(buffer, L"0x1.81cp+13"));
	swprintf(buffer, BUFFER_SIZE, L"%.3A", 12345.0);
	assert(!wcscmp(buffer, L"0X1.81CP+13"));

	test_roundtrip(0.00012345, L"%a", L"%lf", L"0x1.02e4b6ce5dc68p-13", 1);
	test_roundtrip(-0.00012345, L"%A", L"%lf", L"-0X1.02E4B6CE5DC68P-13", 1);
	test_roundtrip(0.0, L"%a", L"%lf", L"0x0p+0", 1);
	test_roundtrip(-0.0, L"%A", L"%lf", L"-0X0P+0", 1);

	swprintf(buffer, BUFFER_SIZE, L"%a", INFINITY);
	assert(!wcscmp(buffer, L"inf"));
	swprintf(buffer, BUFFER_SIZE, L"%A", INFINITY);
	assert(!wcscmp(buffer, L"INF"));
	swprintf(buffer, BUFFER_SIZE, L"%a", -NAN);
	assert(!wcscmp(buffer, L"-nan"));
	swprintf(buffer, BUFFER_SIZE, L"%A", NAN);
	assert(!wcscmp(buffer, L"NAN"));

	// Test %a/%A padding
	test_roundtrip(10.25, L"%25a", L"%lf", L"                0x1.48p+3", 1);
	test_roundtrip(10.25, L"%-25a", L"%lf", L"0x1.48p+3                ", 1);
	test_roundtrip(10.25, L"%25A", L"%lf", L"                0X1.48P+3", 1);
	test_roundtrip(10.25, L"%-25A", L"%lf", L"0X1.48P+3                ", 1);
	test_roundtrip(-10.25, L"%25a", L"%lf", L"               -0x1.48p+3", 1);
	test_roundtrip(-10.25, L"%-25a", L"%lf", L"-0x1.48p+3               ", 1);

	setlocale(LC_ALL, "en_US.utf8");

	swprintf(buffer, BUFFER_SIZE, L"%'f", 42.69);
	assert(!wcscmp(buffer, L"42.690000"));

	swprintf(buffer, BUFFER_SIZE, L"%'20f", 1337420.69);
	assert(!wcscmp(buffer, L"    1,337,420.690000"));

	swprintf(buffer, BUFFER_SIZE, L"%'-20f", 1337420.69);
	assert(!wcscmp(buffer, L"1,337,420.690000    "));

	swprintf(buffer, BUFFER_SIZE, L"%'-10g", 1337.69);
	assert(!wcscmp(buffer, L"1,337.69  "));

	swprintf(buffer, BUFFER_SIZE, L"%'10g", 1337.69);
	assert(!wcscmp(buffer, L"  1,337.69"));

	return 0;
}

#include <fenv.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define test_roundtrip(val, print_format, scanf_format, expected_string, expected_scanf_ret) { \
	double d = val; \
	sprintf(buf, print_format, val); \
	assert(!strcmp(buf, expected_string)); \
	assert(sscanf(buf, scanf_format, &d) == expected_scanf_ret); \
	assert(d == val); \
}

int main() {
	char buf[64] = { 0 };
	sprintf(buf, "%d", 12);
	assert(!strcmp(buf, "12"));

	sprintf(buf, "%f", 3.14);
	assert(!strcmp(buf, "3.140000"));
	sprintf(buf, "%.1f", 0.0);
	assert(!strcmp(buf, "0.0"));

	// Test %g
	sprintf(buf, "%g", 0.0);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%.2g", 0.01234);
	assert(!strcmp(buf, "0.012"));
	sprintf(buf, "%.3g", 1.1);
	assert(!strcmp(buf, "1.1"));
	sprintf(buf, "%#.5g", 1.1);
	assert(!strcmp(buf, "1.1000"));
	sprintf(buf, "%g", 0.0000123);
	assert(!strcmp(buf, "1.23e-05"));

	// Test %e
	sprintf(buf, "%4.3e", 0.0);
	assert(!strcmp(buf, "0.000e+00"));
	sprintf(buf, "%.3e", 6.9e27);
	assert(!strcmp(buf, "6.900e+27"));

	// Test %c right padding.
	sprintf(buf, "%-2c", 'a');
	assert(!strcmp(buf, "a "));

	// Test %c left padding.
	sprintf(buf, "%2c", 'a');
	assert(!strcmp(buf, " a"));

	// Test %d right padding - mlibc issue #58.
	sprintf(buf, "%-2d", 1);
	assert(!strcmp(buf, "1 "));
	sprintf(buf, "%-2.2d", 1);
	assert(!strcmp(buf, "01"));
	sprintf(buf, "%-3.2d", 1);
	assert(!strcmp(buf, "01 "));
	sprintf(buf, "%-3.2d", 12);
	assert(!strcmp(buf, "12 "));
	sprintf(buf, "%-3.2d", 123);
	assert(!strcmp(buf, "123"));
	sprintf(buf, "%-3.2u", 12);
	assert(!strcmp(buf, "12 "));

	// Test %d left padding.
	sprintf(buf, "%2d", 1);
	assert(!strcmp(buf, " 1"));
	sprintf(buf, "%3.2d", 1);
	assert(!strcmp(buf, " 01"));
	sprintf(buf, "%3.2d", 12);
	assert(!strcmp(buf, " 12"));
	sprintf(buf, "%3.2d", 123);
	assert(!strcmp(buf, "123"));
	sprintf(buf, "%3.2u", 12);
	assert(!strcmp(buf, " 12"));
	// Disable -Wformat here since we deliberately induce some warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	sprintf(buf, "%04.3u", 12);
	assert(!strcmp(buf, " 012"));
#pragma GCC diagnostic pop
	sprintf(buf, "%+04d", 0);
	assert(!strcmp(buf, "+000"));

	// Test %f padding.
	// TODO: Test printing of huge numbers (larger than 2^64)
	sprintf(buf, "%2.f", 1.2);
	assert(!strcmp(buf, " 1"));
	sprintf(buf, "%2.f", 12.3);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%5.2f", 1.0);
	assert(!strcmp(buf, " 1.00"));
	sprintf(buf, "%.1f", -4.0);
	assert(!strcmp(buf, "-4.0"));
	sprintf(buf, "%-3.f", 8.0);
	assert(!strcmp(buf, "8  "));
	sprintf(buf, "%4f", INFINITY);
	assert(!strcmp(buf, " inf") || !strcmp(buf, "infinity"));
	sprintf(buf, "%4f", NAN);
	assert(!strcmp(buf, " nan"));
	sprintf(buf, "%4F", INFINITY);
	assert(!strcmp(buf, " INF") || !strcmp(buf, "INFINITY"));
	sprintf(buf, "%4F", NAN);
	assert(!strcmp(buf, " NAN"));
	sprintf(buf, "%05.2f", 1.0);
	assert(!strcmp(buf, "01.00"));
	sprintf(buf, "%09f", INFINITY); // 0 ignored when padding infs
	assert(!strcmp(buf, "      inf") || !strcmp(buf, " infinity"));

	// Test %f rounding
	sprintf(buf, "%5.2f", 1.2);
	assert(!strcmp(buf, " 1.20"));
	sprintf(buf, "%5.2f", 1.23);
	assert(!strcmp(buf, " 1.23"));
	sprintf(buf, "%5.2f", 1.234);
	assert(!strcmp(buf, " 1.23"));
	sprintf(buf, "%5.2f", 12.345);
	assert(!strcmp(buf, "12.35"));
	sprintf(buf, "%-5.2f", 1.2);
	assert(!strcmp(buf, "1.20 "));

	// Test '+' and ' ' flags - mlibc issue #229.
	// Disable -Wformat here since we deliberately induce some warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	sprintf(buf, "%+d", 12);
	assert(!strcmp(buf, "+12"));
	sprintf(buf, "% d", 12);
	assert(!strcmp(buf, " 12"));
	sprintf(buf, "% +d", 12);
	assert(!strcmp(buf, "+12"));
	sprintf(buf, "%+ d", 12);
	assert(!strcmp(buf, "+12"));
	sprintf(buf, "%+d", -12);
	assert(!strcmp(buf, "-12"));
	sprintf(buf, "% d", -12);
	assert(!strcmp(buf, "-12"));
	sprintf(buf, "% +d", -12);
	assert(!strcmp(buf, "-12"));
	sprintf(buf, "%+ d", -12);
	assert(!strcmp(buf, "-12"));
#pragma GCC diagnostic pop

	// Test '#' flag.
	// TODO: Test with a, A, e, E, f, F, g, G conversions.
	sprintf(buf, "%#x", 12);
	assert(!strcmp(buf, "0xc"));
	sprintf(buf, "%#X", 12);
	assert(!strcmp(buf, "0XC"));
	sprintf(buf, "%#o", 12);
	assert(!strcmp(buf, "014"));

	sprintf(buf, "%#x", 0);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%#X", 0);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%#o", 0);
	assert(!strcmp(buf, "0"));

	// Disable -Wformat here because the compiler might not know about the b specifier.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	sprintf(buf, "%#b", 12);
	assert(!strcmp(buf, "0b1100"));
	sprintf(buf, "%#B", 12);
	assert(!strcmp(buf, "0B1100"));
	sprintf(buf, "%#b", 0);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%#B", 0);
	assert(!strcmp(buf, "0"));
#pragma GCC diagnostic pop

	// Test 'd' with different size mods to see
	// if they work
	sprintf(buf, "%d", 12);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%ld", 12L);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%lld", 12LL);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%zd", (size_t)12);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%hd", (short) 12);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%hhd", (char) 12);
	assert(!strcmp(buf, "12"));

	// Test 'x' with different size mods to see
	// if they work
	sprintf(buf, "%x", 12);
	assert(!strcmp(buf, "c"));
	sprintf(buf, "%lx", 12L);
	assert(!strcmp(buf, "c"));
	sprintf(buf, "%llx", 12LL);
	assert(!strcmp(buf, "c"));
	sprintf(buf, "%zx", (size_t)12);
	assert(!strcmp(buf, "c"));
	sprintf(buf, "%hx", (unsigned short) 12);
	assert(!strcmp(buf, "c"));
	sprintf(buf, "%hhx", (unsigned char) 12);
	assert(!strcmp(buf, "c"));

	// Test 'X' with different size mods to see
	// if they work
	sprintf(buf, "%X", 12);
	assert(!strcmp(buf, "C"));
	sprintf(buf, "%lX", 12L);
	assert(!strcmp(buf, "C"));
	sprintf(buf, "%llX", 12LL);
	assert(!strcmp(buf, "C"));
	sprintf(buf, "%zX", (size_t)12);
	assert(!strcmp(buf, "C"));
	sprintf(buf, "%hX", (unsigned short) 12);
	assert(!strcmp(buf, "C"));
	sprintf(buf, "%hhX", (unsigned char) 12);
	assert(!strcmp(buf, "C"));

	// Test 'o' with different size mods to see
	// if they work
	sprintf(buf, "%o", 12);
	assert(!strcmp(buf, "14"));
	sprintf(buf, "%lo", 12L);
	assert(!strcmp(buf, "14"));
	sprintf(buf, "%llo", 12LL);
	assert(!strcmp(buf, "14"));
	sprintf(buf, "%zo", (size_t)12);
	assert(!strcmp(buf, "14"));
	sprintf(buf, "%ho", (unsigned short) 12);
	assert(!strcmp(buf, "14"));
	sprintf(buf, "%hho", (unsigned char) 12);
	assert(!strcmp(buf, "14"));

	// Disable -Wformat here because the compiler might not know about the b specifier.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	// Test 'b' with different size mods to see
	// if they work
	sprintf(buf, "%b", 12);
	assert(!strcmp(buf, "1100"));
	sprintf(buf, "%lb", 12L);
	assert(!strcmp(buf, "1100"));
	sprintf(buf, "%llb", 12LL);
	assert(!strcmp(buf, "1100"));
	sprintf(buf, "%zb", (size_t)12);
	assert(!strcmp(buf, "1100"));
	sprintf(buf, "%hb", (unsigned short) 12);
	assert(!strcmp(buf, "1100"));
	sprintf(buf, "%hhb", (unsigned char) 12);
	assert(!strcmp(buf, "1100"));
#pragma GCC diagnostic pop

	// Test %n$ syntax.
	sprintf(buf, "%1$d", 12);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%1$d %1$d", 12);
	assert(!strcmp(buf, "12 12"));
	sprintf(buf, "%1$d %2$d %1$d", 12, 14);
	assert(!strcmp(buf, "12 14 12"));
	sprintf(buf, "%1$d %2$s %2$s", 12, "foo");
	assert(!strcmp(buf, "12 foo foo"));

	sprintf(buf, "%1$s%5$s%6$s%3$s%4$s%2$s", "a", "b", "u", "n", "h", "N");
	assert(!strcmp(buf, "ahNunb"));

	// test negative precision
	sprintf(buf, "%.*g", -2, 15.1234567);
	assert(!strcmp(buf, "15.1235"));
	sprintf(buf, "%*g", -5, 15.1);
	assert(!strcmp(buf, "15.1 "));
	// Disable -Wformat as the compiler rightfully warns that the '0' flag is ignored
	// when the '0' flag is used
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	sprintf(buf, "%0-6g", 69.2);
	assert(!strcmp(buf, "69.2  "));
#pragma GCC diagnostic pop

	// More extensive floating point tests.
	// Test zero and negative zero
	sprintf(buf, "%f", 0.0);
	assert(!strcmp(buf, "0.000000"));
	sprintf(buf, "%f", -0.0);
	assert(!strcmp(buf, "-0.000000"));
	sprintf(buf, "%e", 0.0);
	assert(!strcmp(buf, "0.000000e+00"));
	sprintf(buf, "%e", -0.0);
	assert(!strcmp(buf, "-0.000000e+00"));
	sprintf(buf, "%g", 0.0);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%g", -0.0);
	assert(!strcmp(buf, "-0"));

	// Test rounding
	sprintf(buf, "%.2f", 1.235);
	assert(!strcmp(buf, "1.24"));
	sprintf(buf, "%.2f", -1.235);
	assert(!strcmp(buf, "-1.24"));
	// 1.2349999999999998 is a double literal that is slightly less than 1.235
	sprintf(buf, "%.2f", 1.2349999999999998);
	assert(!strcmp(buf, "1.23"));
	sprintf(buf, "%.0f", 0.49);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%.0f", -0.49);
	assert(!strcmp(buf, "-0"));

	// Test round-to-even behavior
	fesetround(FE_TONEAREST);

	sprintf(buf, "%.0f", 0.5);
	assert(!strcmp(buf, "0"));
	sprintf(buf, "%.0f", -0.5);
	assert(!strcmp(buf, "-0"));
	sprintf(buf, "%.0f", 0.51);
	assert(!strcmp(buf, "1"));
	sprintf(buf, "%.0f", -0.51);
	assert(!strcmp(buf, "-1"));
	sprintf(buf, "%.0f", 2.5);
	assert(!strcmp(buf, "2"));
	sprintf(buf, "%.0f", -2.5);
	assert(!strcmp(buf, "-2"));
	sprintf(buf, "%.0f", 2.51);
	assert(!strcmp(buf, "3"));
	sprintf(buf, "%.0f", -2.51);
	assert(!strcmp(buf, "-3"));

	// Test %e/%E
	sprintf(buf, "%.3e", 12345.0);
	assert(!strcmp(buf, "1.234e+04"));
	sprintf(buf, "%.3E", 12345.0);
	assert(!strcmp(buf, "1.234E+04"));
	sprintf(buf, "%.3e", 0.00012345);
	assert(!strcmp(buf, "1.234e-04"));
	sprintf(buf, "%.3E", -0.00012345);
	assert(!strcmp(buf, "-1.234E-04"));

	// Test %g/%G behavior
	sprintf(buf, "%g", 123456.0); // Should not use e-notation
	assert(!strcmp(buf, "123456"));
	sprintf(buf, "%g", 1234567.0); // Should use e-notation
	assert(!strcmp(buf, "1.23457e+06"));
	sprintf(buf, "%g", 12345.0);
	assert(!strcmp(buf, "12345"));
	sprintf(buf, "%g", 1.2345);
	assert(!strcmp(buf, "1.2345"));
	sprintf(buf, "%g", 0.00012345); // Should not use e-notation
	assert(!strcmp(buf, "0.00012345"));
	sprintf(buf, "%g", 0.000012345); // Should use e-notation
	assert(!strcmp(buf, "1.2345e-05"));
	sprintf(buf, "%12g", -1.2345);
	assert(!strcmp(buf, "     -1.2345"));

	// %g precision
	sprintf(buf, "%.3g", 12345.0);
	assert(!strcmp(buf, "1.23e+04"));
	sprintf(buf, "%.3G", 12345.0);
	assert(!strcmp(buf, "1.23E+04"));
	sprintf(buf, "%.3g", 1.234);
	assert(!strcmp(buf, "1.23"));
	sprintf(buf, "%.3g", 0.001234);
	assert(!strcmp(buf, "0.00123"));
	sprintf(buf, "%.3g", 0.0001234); // Should not use e-notation
	assert(!strcmp(buf, "0.000123"));
	sprintf(buf, "%.3g", 0.00001234); // Should use e-notation
	assert(!strcmp(buf, "1.23e-05"));
	sprintf(buf, "%08.1g", 69.2);
	assert(!strcmp(buf, "0007e+01"));
	sprintf(buf, "%#08.4g", 69.29854);
	assert(!strcmp(buf, "00069.30"));

	// %g trailing zeros
	sprintf(buf, "%g", 1.200);
	assert(!strcmp(buf, "1.2"));
	sprintf(buf, "%#g", 1.200);
	assert(!strcmp(buf, "1.20000"));
	sprintf(buf, "%g", 1200.0);
	assert(!strcmp(buf, "1200"));
	sprintf(buf, "%#g", 1200.0);
	assert(!strcmp(buf, "1200.00"));

	// %a + %A
	test_roundtrip(3.14, "%a", "%lf", "0x1.91eb851eb851fp+1", 1);
	test_roundtrip(3.14, "%A", "%lf", "0X1.91EB851EB851FP+1", 1);
	test_roundtrip(12345.0, "%a", "%lf", "0x1.81c8p+13", 1);
	test_roundtrip(12345.0, "%A", "%lf", "0X1.81C8P+13", 1);

	sprintf(buf, "%.3a", 12345.0);
	assert(!strcmp(buf, "0x1.81cp+13"));
	sprintf(buf, "%.3A", 12345.0);
	assert(!strcmp(buf, "0X1.81CP+13"));

	test_roundtrip(0.00012345, "%a", "%lf", "0x1.02e4b6ce5dc68p-13", 1);
	test_roundtrip(-0.00012345, "%A", "%lf", "-0X1.02E4B6CE5DC68P-13", 1);
	test_roundtrip(0.0, "%a", "%lf", "0x0p+0", 1);
	test_roundtrip(-0.0, "%A", "%lf", "-0X0P+0", 1);

	sprintf(buf, "%a", INFINITY);
	assert(!strcmp(buf, "inf"));
	sprintf(buf, "%A", INFINITY);
	assert(!strcmp(buf, "INF"));
	sprintf(buf, "%a", -NAN);
	assert(!strcmp(buf, "-nan"));
	sprintf(buf, "%A", NAN);
	assert(!strcmp(buf, "NAN"));

	// Test %a/%A padding
	test_roundtrip(10.25, "%25a", "%lf", "                0x1.48p+3", 1);
	test_roundtrip(10.25, "%-25a", "%lf", "0x1.48p+3                ", 1);
	test_roundtrip(10.25, "%25A", "%lf", "                0X1.48P+3", 1);
	test_roundtrip(10.25, "%-25A", "%lf", "0X1.48P+3                ", 1);
	test_roundtrip(-10.25, "%25a", "%lf", "               -0x1.48p+3", 1);
	test_roundtrip(-10.25, "%-25a", "%lf", "-0x1.48p+3               ", 1);

	return 0;
}

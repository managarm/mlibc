#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

int main() {
	char buf[11] = { 0 };
	sprintf(buf, "%d", 12);
	assert(!strcmp(buf, "12"));
	sprintf(buf, "%f", 3.14);
	assert(!strcmp(buf, "3.140000"));

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
	// TODO: We don't yet round properly
	// sprintf(buf, "%5.2f", 1.2);
	// assert(!strcmp(buf, " 1.20"));
	// sprintf(buf, "%5.2f", 1.23);
	// assert(!strcmp(buf, " 1.23"));
	// sprintf(buf, "%5.2f", 1.234);
	// assert(!strcmp(buf, " 1.23"));
	// sprintf(buf, "%5.2f", 12.345);
	// assert(!strcmp(buf, "12.35"));
	// sprintf(buf, "%-5.2f", 1.2);
	// assert(!strcmp(buf, "1.20 "));

	// Test '+' and ' ' flags - mlibc issue #229.
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

	return 0;
}

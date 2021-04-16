#include <stdio.h>
#include <assert.h>
#include <math.h>

int main() {
	char buf[11];
	sprintf(buf, "%d", 12);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '\0');
	sprintf(buf, "%f", 3.14);
	assert(buf[0] == '3' && buf[1] == '.' && buf[2] == '1'
			&& buf[3] == '4' && buf[4] == '0' && buf[5] == '0'
			&& buf[6] == '0' && buf[7] == '0' && buf[8] == '\0');

	// Test %c right padding.
	sprintf(buf, "%-2c", 'a');
	assert(buf[0] == 'a' && buf[1] == ' ' && buf[2] == '\0');

	// Test %c left padding.
	sprintf(buf, "%2c", 'a');
	assert(buf[0] == ' ' && buf[1] == 'a' && buf[2] == '\0');

	// Test %d right padding - mlibc issue #58.
	sprintf(buf, "%-2d", 1);
	assert(buf[0] == '1' && buf[1] == ' ' && buf[2] == '\0');
	sprintf(buf, "%-2.2d", 1);
	assert(buf[0] == '0' && buf[1] == '1' && buf[2] == '\0');
	sprintf(buf, "%-3.2d", 1);
	assert(buf[0] == '0' && buf[1] == '1' && buf[2] == ' '
			&& buf[3] == '\0');
	sprintf(buf, "%-3.2d", 12);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == ' '
			&& buf[3] == '\0');
	sprintf(buf, "%-3.2d", 123);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '3'
			&& buf[3] == '\0');
	sprintf(buf, "%-3.2u", 12);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == ' '
			&& buf[3] == '\0');

	// Test %d left padding.
	sprintf(buf, "%2d", 1);
	assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '\0');
	sprintf(buf, "%3.2d", 1);
	assert(buf[0] == ' ' && buf[1] == '0' && buf[2] == '1'
			&& buf[3] == '\0');
	sprintf(buf, "%3.2d", 12);
	assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '2'
			&& buf[3] == '\0');
	sprintf(buf, "%3.2d", 123);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '3'
			&& buf[3] == '\0');
	sprintf(buf, "%3.2u", 12);
	assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '2'
			&& buf[3] == '\0');

	// Test %f padding.
	// TODO: Test printing of huge numbers (larger than 2^64)
	sprintf(buf, "%2.f", 1.2);
	assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '\0');
	sprintf(buf, "%2.f", 12.3);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '\0');
	sprintf(buf, "%5.2f", 1.0);
	assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '.' && buf[3] == '0'
			&& buf[4] == '0' && buf[5] == '\0');
	sprintf(buf, "%.1f", -4.0);
	assert(buf[0] == '-' && buf[1] == '4' && buf[2] == '.' && buf[3] == '0'
			&& buf[4] == '\0');
	sprintf(buf, "%-3.f", 8.0);
	assert(buf[0] == '8' && buf[1] == ' ' && buf[2] == ' ' && buf[4] == '\0');
	sprintf(buf, "%4f", INFINITY);
	assert((buf[0] == ' ' || buf[0] == 'i') && (buf[1] == 'i' || buf[2] == 'n'));
	sprintf(buf, "%4f", NAN);
	assert(buf[0] == ' ' && buf[1] == 'n' && buf[2] == 'a' && buf[3] == 'n' && buf[4] == '\0');
	sprintf(buf, "%4F", INFINITY);
	assert((buf[0] == ' ' || buf[0] == 'I') && (buf[1] == 'I' || buf[2] == 'N'));
	sprintf(buf, "%4F", NAN);
	assert(buf[0] == ' ' && buf[1] == 'N' && buf[2] == 'A' && buf[3] == 'N' && buf[4] == '\0');
	// TODO: We don't yet round properly
	// sprintf(buf, "%5.2f", 1.2);
	// assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '.' && buf[3] == '2'
	// 		&& buf[4] == '0' && buf[5] == '\0');
	// sprintf(buf, "%5.2f", 1.23);
	// assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '.' && buf[3] == '2'
	// 		&& buf[4] == '3' && buf[5] == '\0');
	// sprintf(buf, "%5.2f", 1.234);
	// assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '.' && buf[3] == '2'
	// 		&& buf[4] == '3' && buf[5] == '\0');
	// sprintf(buf, "%5.2f", 12.345);
	// assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '.' && buf[3] == '3'
	// 		&& buf[4] == '5' && buf[5] == '\0');
	// sprintf(buf, "%-5.2f", 1.2);
	// assert(buf[0] == '1' && buf[1] == '.' && buf[2] == '2' && buf[3] == '0'
	// 		&& buf[4] == ' ' && buf[5] == '\0');

	// Test '+' and ' ' flags - mlibc issue #229.
	sprintf(buf, "%+d", 12);
	assert(buf[0] == '+' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "% d", 12);
	assert(buf[0] == ' ' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "% +d", 12);
	assert(buf[0] == '+' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "%+ d", 12);
	assert(buf[0] == '+' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "%+d", -12);
	assert(buf[0] == '-' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "% d", -12);
	assert(buf[0] == '-' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "% +d", -12);
	assert(buf[0] == '-' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');
	sprintf(buf, "%+ d", -12);
	assert(buf[0] == '-' && buf[1] == '1' && buf[2] == '2' && buf[3] == '\0');

	// Test '#' flag.
	// TODO: Test with a, A, e, E, f, F, g, G conversions.
	sprintf(buf, "%#x", 12);
	assert(buf[0] == '0' && buf[1] == 'x' && buf[2] == 'c' && buf[3] == '\0');
	sprintf(buf, "%#X", 12);
	assert(buf[0] == '0' && buf[1] == 'X' && buf[2] == 'C' && buf[3] == '\0');
	sprintf(buf, "%#o", 12);
	assert(buf[0] == '0' && buf[1] == '1' && buf[2] == '4' && buf[3] == '\0');
	sprintf(buf, "%#x", 0);
	assert(buf[0] == '0' && buf[1] == '\0');
	sprintf(buf, "%#X", 0);
	assert(buf[0] == '0' && buf[1] == '\0');
	sprintf(buf, "%#o", 0);
	assert(buf[0] == '0' && buf[1] == '\0');

	return 0;
}

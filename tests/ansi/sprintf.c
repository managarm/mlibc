#include <stdio.h>
#include <assert.h>

int main() {
	char buf[11];
	sprintf(buf, "%d", 12);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '\0');
	sprintf(buf, "%f", 3.14);
	assert(buf[0] == '3' && buf[1] == '.' && buf[2] == '1'
			&& buf[3] == '4' && buf[4] == '\0');

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

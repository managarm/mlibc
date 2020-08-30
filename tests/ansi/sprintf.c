#include <stdio.h>
#include <assert.h>

int main() {
	char buf[11];
	sprintf(buf, "%d", 12);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '\0');
	sprintf(buf, "%f", 3.14);
	assert(buf[0] == '3' && buf[1] == '.' && buf[2] == '1'
			&& buf[3] == '4' && buf[4] == '\0');
	return 0;
}

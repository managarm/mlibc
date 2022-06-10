#include <string.h>
#include <assert.h>

int main() {
	char buf[4];

	// stpncpy
	assert(stpncpy(buf, "", 4) == buf);
	assert(!strcmp(buf, ""));
	
	assert(stpncpy(buf, "123", 4) == buf + 3);
	assert(!strcmp(buf, "123"));
	
	assert(stpncpy(buf, "12", 4) == buf + 2);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '\0' && buf[3] == '\0');

	assert(stpncpy(buf, "123456", 4) == buf + 4);
	assert(buf[0] == '1' && buf[1] == '2' && buf[2] == '3' && buf[3] == '4');

	// stpcpy
	assert(stpcpy(buf, "") == buf);
	assert(!strcmp(buf, ""));
	
	assert(stpcpy(buf, "12") == buf + 2);
	assert(!strcmp(buf, "12"));
	
	assert(stpcpy(buf, "123") == buf + 3);
	assert(!strcmp(buf, "123"));
}

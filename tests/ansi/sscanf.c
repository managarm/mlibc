#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
	int x = 0;
	char buf1[] = "12345";
	sscanf(buf1, "%d", &x);
	assert(x == 12345);

	// From dsda-doom
	char buf2[] = "process_priority               0\n";
	char def[80], strparm[128];
	memset(def, '!', 80);
	memset(strparm, '!', 128);
	sscanf(buf2, "%s %[^\n]\n", def, strparm);
	assert(!strcmp(def, "process_priority"));
	assert(!strcmp(strparm, "0"));

	return 0;
}

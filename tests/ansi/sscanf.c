#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
	int x = 0;
	char buf1[] = "12345";
	sscanf(buf1, "%d", &x);
	assert(x == 12345);

	char c;
	int n1;
	int n2;
	char buf3[] = "z$ 7 5 440";;
	int count = sscanf(buf3, "%*c%c %d %*d %d", &c, &n1, &n2);
	assert(count == 3);
	assert(c == '$');
	assert(n1 == 7);
	assert(n2 = 440);

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

#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
	{
		int x = 0;
		char buf[] = "12345";
		sscanf(buf, "%d", &x);
		assert(x == 12345);
	}

	{
		char c;
		int n1;
		int n2;
		char buf[] = "z$ 7 5 440";;
		int count = sscanf(buf, "%*c%c %d %*d %d", &c, &n1, &n2);
		assert(count == 3);
		assert(c == '$');
		assert(n1 == 7);
		assert(n2 == 440);
	}

	{
		// From dsda-doom
		char buf[] = "process_priority               0\n";
		char def[80], strparm[128];
		memset(def, '!', 80);
		memset(strparm, '!', 128);
		sscanf(buf, "%s %[^\n]\n", def, strparm);
		assert(!strcmp(def, "process_priority"));
		assert(!strcmp(strparm, "0"));
	}

	{
		char buf[] = "fffffffff100";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0xfffffffff100);
	}

	{
		char buf[] = "410dc000";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0x410dc000);
	}
	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct format_test_cases {
	const char *format;
	const char *data;
	int expected_int;
	enum {
		T_INT,
		T_UINT,
		T_CHAR,
		T_NONE,
	} type;
	int ret;
} formats[] = {
	{"%i", "0x420", 0x420, T_INT, 1},
	{"%i", "0420", 0420, T_INT, 1},
	{"%i", "420", 420, T_INT, 1},
	{"%i", "-420", -420, T_INT, 1},
	{"%d", "-12345", -12345, T_INT, 1},
	{"%u", "69", 69, T_UINT, 1},
	{"%u", "0420", 420, T_UINT, 1},
	{"%o", "0420", 0420, T_UINT, 1},
	{"%x", "0xCB7", 0xCB7, T_UINT, 1},
#ifndef USE_HOST_LIBC
	{"%b", "0b1011", 0b1011, T_UINT, 1},
	{"%b", "0B1011", 0b1011, T_UINT, 1},
#endif
	{"%%", "%", 0, T_NONE, 0},
	{"%c", "         I am not a fan of this solution.", ' ', T_CHAR, 1},
	{" %c", "           CBT (capybara therapy)", 'C', T_CHAR, 1},
	{"%d %d %d", "  111111 I<3Managarm 1234", 111111, T_UINT, 1},
	{"%c %d", "C", 'C', T_CHAR, 1}
};

#pragma GCC diagnostic ignored "-Wformat-security"

static void test_matrix() {
	for(size_t i = 0; i < (sizeof(formats) / sizeof(*formats)); i++) {
		struct format_test_cases *f = &formats[i];
		int ret = -1;
		int data_int;
		unsigned int data_uint;
		char data_char;

		switch(f->type) {
			case T_INT: {
				ret = sscanf(f->data, f->format, &data_int);
				assert(data_int == f->expected_int);
				break;
			}
			case T_UINT: {
				ret = sscanf(f->data, f->format, &data_uint);
				assert(data_uint == (unsigned int) f->expected_int);
				break;
			}
			case T_CHAR: {
				ret = sscanf(f->data, f->format, &data_char);
				assert(data_char == (unsigned char) f->expected_int);
				break;
			}
			case T_NONE: {
				ret = sscanf(f->data, f->format);
				break;
			}

		}

		assert(ret == f->ret);
	}
}

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
		char buf[] = "fffff100";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0xfffff100);
	}

#if !defined(__i386__) && !defined(__m68k__)
	{
		char buf[] = "fffffffff100";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0xfffffffff100);
	}
#endif

	{
		char buf[] = "410dc000";
		unsigned long y = 0;
		sscanf(buf, "%lx", &y);
		assert(y == 0x410dc000);
	}

	{
		// From webkitgtk
		char buf[] = "MemTotal:       16299664 kB\n";
		char token[51] = {0};
		size_t amount = 0;
		int ret = sscanf(buf, "%50s%zukB", token, &amount);
		assert(ret == 2);
		assert(!strcmp(token, "MemTotal:"));
		assert(amount == 16299664);
	}

	{
		char buf[] = "SIGINT";
		int sig;
		int ret = sscanf(buf, "%d", &sig);
		assert(!ret);
	}

	{
		char buf[50];
		int ret = sscanf("", "%s", buf);
		assert(ret == EOF);
	}

	{
		char *str = NULL;
		int ret = sscanf("Managarm", "%ms", &str);
		assert(ret == 1);
		assert(str != NULL);
		assert(!strcmp(str, "Managarm"));
		free(str);
	}

	test_matrix();

	return 0;
}

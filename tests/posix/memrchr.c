#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main () {
	char str[] = "The Last Supper by Leonardo da Vinci";
	char *str_temp;

	// Test strstr
	str_temp = strstr(str, "Supper"); /* Find a substring in the string */
	assert(!strcmp(str_temp, "Supper by Leonardo da Vinci"));

	/* Following calls use memory APIs for the above tasks */
	// Test memchr
	str_temp = (char *)memchr((void *)str, 'L', strlen(str));
	assert(!strcmp(str_temp, "Last Supper by Leonardo da Vinci"));

	// Test memrchr
	str_temp = (char *)memrchr((void *)str, 'L', strlen(str));
	assert(!strcmp(str_temp, "Leonardo da Vinci"));
	return 0;
}

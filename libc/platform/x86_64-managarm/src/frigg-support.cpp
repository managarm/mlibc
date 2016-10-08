
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/support.hpp>

#include <hel.h>
#include <hel-syscalls.h>

void friggPrintCritical(char c) {
	helLog(&c, 1);
}

void friggPrintCritical(const char *str) {
	int len = 0;
	for(int i = 0; str[i]; i++)
		len++;
	helLog(str, len);
}

void friggPanic() {
	const char *str = "Panic in mlibc";
	int len = 0;
	for(int i = 0; str[i]; i++)
		len++;
	helPanic(str, len);
}


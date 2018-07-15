
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/support.hpp>

#include <hel.h>
#include <hel-syscalls.h>

void friggBeginLog() { }
void friggEndLog() { }

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

extern "C" void frg_panic(const char *mstr) {
	int mlen = 0;
	while(mstr[mlen])
		mlen++;
	helLog(mstr, mlen);

	const char *str = "Panic in mlibc";
	int len = 0;
	while(str[len])
		len++;
	helPanic(str, len);
}


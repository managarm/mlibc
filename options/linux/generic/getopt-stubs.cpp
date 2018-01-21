
#include <getopt.h>

#include <frigg/debug.hpp>
#include <mlibc/ensure.h>

int getopt_long(int, char *const[], const char *, const struct option *, int *) {
	frigg::infoLogger() << "\e[31mmlibc: getopt_long() is only a stub\e[39m" << frigg::endLog;
	return -1;
}

int getopt_long_only(int, char *const[], const char *, const struct option *, int *) {
	frigg::infoLogger() << "\e[31mmlibc: getopt_long_only() is only a stub\e[39m" << frigg::endLog;
	return -1;
}


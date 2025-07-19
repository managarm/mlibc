#include <getopt.h>
#include <mlibc/getopt.hpp>

int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	return getopt_common(argc, argv, optstring, longopts, longindex, mlibc::GetoptMode::Long);
}

int getopt_long_only(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	return getopt_common(argc, argv, optstring, longopts, longindex, mlibc::GetoptMode::LongOnly);
}

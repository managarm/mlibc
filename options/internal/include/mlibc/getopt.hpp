#ifndef MLIBC_GETOPT
#define MLIBC_GETOPT

struct option;

namespace mlibc {

enum GetoptMode {
	Short,
	Long,
	LongOnly,
};

int getopt_common(int argc, char * const argv[], const char *optstring,
	const struct option *longopts, int *longindex, enum GetoptMode mode);

} // namespace mlibc

#endif // MLIBC_GETOPT

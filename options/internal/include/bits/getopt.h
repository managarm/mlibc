#ifndef _MLIBC_INTERNAL_BITS_GETOPT
#define _MLIBC_INTERNAL_BITS_GETOPT

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

#define no_argument 0
#define required_argument 1
#define optional_argument 2

#endif /* _MLIBC_INTERNAL_BITS_GETOPT */

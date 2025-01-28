
#ifndef  _GETOPT_H
#define  _GETOPT_H

#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

#ifndef __MLIBC_ABI_ONLY

extern char **environ;
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
#if __MLIBC_BSD_OPTION
extern int optreset;
#endif /*__MLIBC_BSD_OPTION */

int getopt(int __argc, char *const __argv[], const char *__optstring);
int getopt_long(int __argc, char *const __argv[], const char *__optstring,
		const struct option *__longopts, int *__longindex);
int getopt_long_only(int __argc, char *const __argv[], const char *__optstring,
		const struct option *__longopts, int *__longindex);

#endif /* !__MLIBC_ABI_ONLY */

#define no_argument 0
#define required_argument 1
#define optional_argument 2

#ifdef __cplusplus
}
#endif

#endif /* _GETOPT_H */



#ifndef  _GETOPT_H
#define  _GETOPT_H

#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/getopt.h>

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

#ifdef __cplusplus
}
#endif

#endif /* _GETOPT_H */


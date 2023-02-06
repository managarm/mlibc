#include <getopt.h>

#if __MLIBC_GLIBC_OPTION

int __optreset = 0;
extern int optreset __attribute__((weak, alias("__optreset")));

#endif //__MLIBC_GLIBC_OPTION

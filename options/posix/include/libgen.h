
#ifndef _LIBGEN_H
#define _LIBGEN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(basename) && defined(_GNU_SOURCE)
/* see: ./options/ansi/include/string.h, search for __mlibc_gnu_basename */
# undef basename
#endif

char *basename(char *);
#define basename basename
char *dirname(char *);

#ifdef __cplusplus
}
#endif

#endif // _LIBGEN_H



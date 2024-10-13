
#ifndef _LIBGEN_H
#define _LIBGEN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(basename) && defined(_GNU_SOURCE)
/* see: ./options/ansi/include/string.h, search for __mlibc_gnu_basename */
# undef basename
#endif

#ifndef __MLIBC_ABI_ONLY

char *basename(char *__path);
#define basename basename
char *dirname(char *__path);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _LIBGEN_H */



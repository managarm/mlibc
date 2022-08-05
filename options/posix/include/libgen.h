
#ifndef _LIBGEN_H
#define _LIBGEN_H

#ifdef __cplusplus
extern "C" {
#endif

char *__mlibc_xpg_basename(char *);
#define basename __mlibc_xpg_basename
char *dirname(char *);

#ifdef __cplusplus
}
#endif

#endif // _LIBGEN_H



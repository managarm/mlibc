#ifndef NL_TYPES_H
#define NL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *nl_catd;

#ifndef __MLIBC_ABI_ONLY

char *catgets(nl_catd __catalog, int __set, int __number, const char *__string);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* NL_TYPES_H */

#ifndef _MODULE_H
#define _MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/*
 * Musl adds these, even though they aren't specified, but doesn't export them.
 * See https://github.com/bminor/musl/commit/2169265ec6c902cd460bf96a1a0b5103657a4954
 * for more information and the rationale behind it.
 * For our infrastructure, we expose them, and make it call into the sysdeps.
 */
int init_module(void *__module, unsigned long __len, const char *__args);
int delete_module(const char *__name, unsigned __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MODULE_H */

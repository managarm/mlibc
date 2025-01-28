
#ifndef _SEARCH_H
#define _SEARCH_H

#include <stddef.h>
#include <bits/search.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  preorder,
  postorder,
  endorder,
  leaf
} VISIT;

#if __MLIBC_GLIBC_OPTION && defined(_GNU_SOURCE)
#include <bits/glibc/glibc_search.h>
#endif

#ifndef __MLIBC_ABI_ONLY

void *tsearch(const void *__key, void **__root, int(*__compar)(const void *__a, const void *__b));
void *tfind(const void *__key, void *const *__root, int (*__compar)(const void *__a, const void *__b));
void *tdelete(const void *__key, void **__root, int(*__compar)(const void * __a, const void *__b));
void twalk(const void *__key, void (*__action)(const void *__node, VISIT __which, int  __depth));
void tdestroy(void *__root, void (*__free_node)(void *__node));

void *lsearch(const void *__key, void *__base, size_t *__nelp, size_t __width,
		int (*__compar)(const void *__a, const void *__b));
void *lfind(const void *__key, const void *__base, size_t *__nelp,
		size_t __width, int (*__compar)(const void *__a, const void *__b));

int hcreate(size_t __num_entries);
void hdestroy(void);
ENTRY *hsearch(ENTRY __item, ACTION __action);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SEARCH_H */

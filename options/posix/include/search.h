
#ifndef _SEARCH_H
#define _SEARCH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  preorder,
  postorder,
  endorder,
  leaf
} VISIT;

#ifndef __MLIBC_ABI_ONLY

void *tsearch(const void *, void **, int(*compar)(const void *, const void *));
void *tfind(const void *, void *const *, int (*compar)(const void *, const void *));
void *tdelete(const void *, void **, int(*compar)(const void *, const void *));
void twalk(const void *, void (*action)(const void *, VISIT, int));
void tdestroy(void *, void (*free_node)(void *));

void *lsearch(const void *key, void *base, size_t *nelp, size_t width,
		int (*compar)(const void *, const void *));
void *lfind(const void *key, const void *base, size_t *nelp,
		size_t width, int (*compar)(const void *, const void *));

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SEARCH_H

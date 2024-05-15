
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

void *tsearch(const void *, void **, int(*compar)(const void *, const void *));
void *tfind(const void *, void *const *, int (*compar)(const void *, const void *));
void *tdelete(const void *, void **, int(*compar)(const void *, const void *));
void twalk(const void *, void (*action)(const void *, VISIT, int));
void tdestroy(void *, void (*free_node)(void *));

void *lsearch(const void *key, void *base, size_t *nelp, size_t width,
		int (*compar)(const void *, const void *));
void *lfind(const void *key, const void *base, size_t *nelp,
		size_t width, int (*compar)(const void *, const void *));

int hcreate(size_t num_entries);
void hdestroy(void);
ENTRY *hsearch(ENTRY item, ACTION action);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SEARCH_H

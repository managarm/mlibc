
#ifndef _SEARCH_H
#define _SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  preorder,
  postorder,
  endorder,
  leaf
} VISIT;

void *tsearch(const void *, void **, int(*compar)(const void *, const void *));
void *tfind(const void *, void *const *, int (*compar)(const void *, const void *));
void *tdelete(const void *, void **, int(*compar)(const void *, const void *));
void twalk(const void *, void (*action)(const void *, VISIT, int));
void tdestroy(void *, void (*free_node)(void *));

#ifdef __cplusplus
}
#endif

#endif // _SEARCH_H

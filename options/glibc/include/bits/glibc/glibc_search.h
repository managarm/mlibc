#ifndef _GLIBC_SEARCH_H
#define _GLIBC_SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/search.h>

#ifndef __MLIBC_ABI_ONLY

int hcreate_r(size_t __num_entries, struct hsearch_data *__htab);
void hdestroy_r(struct hsearch_data *__htab);
int hsearch_r(ENTRY __item, ACTION __action, ENTRY **__ret, struct hsearch_data *__htab);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _GLIBC_SEARCH_H */

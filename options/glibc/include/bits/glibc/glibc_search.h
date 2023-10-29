#ifndef _GLIBC_SEARCH_H
#define _GLIBC_SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>

typedef enum {
	FIND,
	ENTER
} ACTION;

typedef struct entry {
	char *key;
	void *data;
} ENTRY;

struct _ENTRY;

struct hsearch_data {
	struct _ENTRY *table;
	unsigned int size;
	unsigned int filled;
};

#ifndef __MLIBC_ABI_ONLY

int hcreate(size_t nel);
void hdestroy(void);
ENTRY *hsearch(ENTRY item, ACTION action);

int hsearch_r(ENTRY item, ACTION action, ENTRY **ret, struct hsearch_data *htab);
int hcreate_r(size_t num_elements, struct hsearch_data *htab);
void hdestroy_r(struct hsearch_data *htab);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _GLIBC_SEARCH_H */

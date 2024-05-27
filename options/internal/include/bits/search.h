#ifndef _MLIBC_INTERNAL_SEARCH_H
#define _MLIBC_INTERNAL_SEARCH_H

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

#endif /* _MLIBC_INTERNAL_SEARCH_H */

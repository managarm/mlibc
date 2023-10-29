#include <bits/glibc/glibc_search.h>
#include <stdlib.h>
#include <string.h>

// This code was lifted from musl!

#define MINSIZE 8
#define MAXSIZE ((size_t)-1/2 + 1)

struct _ENTRY {
	ENTRY *entries;
	size_t mask;
	size_t used;
};

static size_t keyhash(char *k) {
	unsigned char *p = (unsigned char *)k;
	size_t h = 0;

	while(*p) {
		h = 31 * h + *p++;
	}
	return h;
}

static int resize(size_t nel, struct hsearch_data *htab) {
	size_t newsize;
	size_t i, j;
	size_t oldsize = htab->table->mask + 1;
	ENTRY *e, *newe;
	ENTRY *oldtab = htab->table->entries;

	if(nel > MAXSIZE) {
		nel = MAXSIZE;
	}

	for(newsize = MINSIZE; newsize < nel; newsize *= 2);
	
	htab->table->entries = (ENTRY *)calloc(newsize, sizeof *htab->table->entries);
	
	if(!htab->table->entries) {
		htab->table->entries = oldtab;
		return 0;
	}
	htab->table->mask = newsize - 1;
	
	if(!oldtab)
		return 1;
	
	for(e = oldtab; e < oldtab + oldsize; e++) {
		if(e->key) {
			for(i = keyhash(e->key),j = 1; ; i += j++) {
				newe = htab->table->entries + (i & htab->table->mask);
				if(!newe->key) {
					break;
				}
			}
			*newe = *e;
		}
	}
	free(oldtab);
	return 1;
}

static ENTRY *lookup(char *key, size_t hash, struct hsearch_data *htab) {
	size_t i, j;
	ENTRY *e;

	for(i = hash, j = 1; ; i += j++) {
		e = htab->table->entries + (i & htab->table->mask);
		if(!e->key || strcmp(e->key, key) == 0) {
			break;
		}
	}
	return e;
}

int hsearch_r(ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *htab) {
	size_t hash = keyhash(item.key);
	ENTRY *e = lookup(item.key, hash, htab);

	if(e->key) {
		*retval = e;
		return 1;
	}
	if(action == FIND) {
		*retval = 0;
		return 0;
	}
	*e = item;
	if(++htab->table->used > htab->table->mask - htab->table->mask / 4) {
		if(!resize(2 * htab->table->used, htab)) {
			htab->table->used--;
			e->key = 0;
			*retval = 0;
			return 0;
		}
		e = lookup(item.key, hash, htab);
	}
	*retval = e;
	return 1;
}

int hcreate_r(size_t nel, struct hsearch_data *htab) {
	int r;

	htab->table = (_ENTRY *)calloc(1, sizeof *htab->table);
	if(!htab->table) {
		return 0;
	}
	r = resize(nel, htab);
	if(r == 0) {
		free(htab->table);
		htab->table = 0;
	}
	return r;
}

void hdestroy_r(struct hsearch_data *htab) {
	if(htab->table) {
		free(htab->table->entries);
	}
	free(htab->table);
	htab->table = 0;
}

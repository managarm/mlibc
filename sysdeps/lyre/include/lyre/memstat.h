#ifndef _LYRE__MEMSTAT_H
#define _LYRE__MEMSTAT_H

#include <stdint.h>
#include <stddef.h>

enum lyre_kmemstat_tag {
    KMEM_ALLOC_UNKNOWN,
    KMEM_ALLOC_VECTOR,
    KMEM_ALLOC_HASHMAP,
    KMEM_ALLOC_STRING,
    KMEM_ALLOC_PAGEMAP,
    KMEM_ALLOC_PROCESS,
    KMEM_ALLOC_THREAD,
    KMEM_ALLOC_RESOURCE,
    KMEM_ALLOC_MISC,
    KMEM_ALLOC_TAG_MAX,
};

struct lyre_kmemstat {
    uint64_t n_phys_total;
    uint64_t n_phys_used;
    uint64_t n_phys_free;
    uint64_t n_phys_reserved;
    uint64_t n_heap_used[KMEM_ALLOC_TAG_MAX];
};

#endif

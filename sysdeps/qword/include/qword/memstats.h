#ifndef _QWORD_MEMSTATS_H
#define _QWORD_MEMSTATS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct memstats {
	size_t total;
	size_t used;
};

int getmemstats(struct memstats *);

#ifdef __cplusplus
}
#endif

#endif // _QWORD_MEMSTATS_H

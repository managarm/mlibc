#ifndef _QWORD_PERFMON_H
#define _QWORD_PERFMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct perfstats {
	uint64_t cpu_time;
	uint64_t syscall_time;
	uint64_t mman_time;
	uint64_t io_time;
};

int perfmon_create();
int perfmon_attach(int);

#ifdef __cplusplus
}
#endif

#endif // _QWORD_PERFMON_H

#ifndef _LINUX_CAPABILITY_H
#define _LINUX_CAPABILITY_H

#include <linux/types.h>

typedef struct __user_cap_header_struct {
	__u32 version;
	int pid;
} *cap_user_header_t;

typedef struct __user_cap_data_struct {
	__u32 effective;
	__u32 permitted;
	__u32 inheritable;
} *cap_user_data_t;

#define CAP_TO_INDEX(x) ((x) >> 5)

#define CAP_KILL 5
#define CAP_SETPCAP 8
#define CAP_SYS_ADMIN 21

#endif // _LINUX_CAPABILITY_H

#ifndef _LINUX_IF_ADDR_H
#define _LINUX_IF_ADDR_H

#include <linux/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ifaddrmsg {
	uint8_t ifa_family;
	uint8_t ifa_prefixlen;
	uint8_t ifa_flags;
	uint8_t ifa_scope;
	uint32_t ifa_index;
};

struct ifa_cacheinfo {
	uint32_t ifa_prefered;
	uint32_t ifa_valid;
	uint32_t cstamp;
	uint32_t tstamp;
};

#ifdef __cplusplus
}
#endif

#endif

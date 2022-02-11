#ifndef _LINUX_IF_ADDRLABEL_H
#define _LINUX_IF_ADDRLABEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <stdint.h>

struct ifaddrlblmsg {
	uint8_t ifal_family;
	uint8_t __ifal_reserved;
	uint8_t ifal_prefixlen;
	uint8_t ifal_flags;
	uint32_t ifal_index;
	uint32_t ifal_seq;
};

enum {
	IFAL_ADDRESS = 1,
	IFAL_LABEL = 2,
	__IFAL_MAX
};

#define IFAL_MAX        (__IFAL_MAX - 1)

#ifdef __cplusplus
}
#endif

#endif

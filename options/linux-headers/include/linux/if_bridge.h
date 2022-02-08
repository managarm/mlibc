#ifndef _LINUX_IF_BRIDGE_H
#define _LINUX_IF_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <stdint.h>

#define BR_STATE_DISABLED 0
#define BR_STATE_LISTENING 1
#define BR_STATE_LEARNING 2
#define BR_STATE_FORWARDING 3
#define BR_STATE_BLOCKING 4

struct br_port_msg {
	uint8_t  family;
	uint32_t ifindex;
};

enum {
	MDBA_SET_ENTRY_UNSPEC,
	MDBA_SET_ENTRY,
	MDBA_SET_ENTRY_ATTRS,
	__MDBA_SET_ENTRY_MAX,
};
#define MDBA_SET_ENTRY_MAX (__MDBA_SET_ENTRY_MAX - 1)

#ifdef __cplusplus
}
#endif

#endif

#ifndef _LINUX_IF_BRIDGE_H
#define _LINUX_IF_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>

#define BR_STATE_DISABLED 0
#define BR_STATE_LISTENING 1
#define BR_STATE_LEARNING 2
#define BR_STATE_FORWARDING 3
#define BR_STATE_BLOCKING 4

struct br_port_msg {
	uint8_t  family;
	uint32_t ifindex;
};

#ifdef __cplusplus
}
#endif

#endif

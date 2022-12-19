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

enum {
	IFLA_BRIDGE_FLAGS,
	IFLA_BRIDGE_MODE,
	IFLA_BRIDGE_VLAN_INFO,
	IFLA_BRIDGE_VLAN_TUNNEL_INFO,
	IFLA_BRIDGE_MRP,
	IFLA_BRIDGE_CFM,
	IFLA_BRIDGE_MST,
	__IFLA_BRIDGE_MAX,
};
#define IFLA_BRIDGE_MAX (__IFLA_BRIDGE_MAX - 1)

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

struct bridge_vlan_info {
	uint16_t flags;
	uint16_t vid;
};

#ifdef __cplusplus
}
#endif

#endif

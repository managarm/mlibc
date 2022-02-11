#ifndef _LINUX_NEIGHBOUR_H
#define _LINUX_NEIGHBOUR_H

#include <linux/types.h>
#include <linux/netlink.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ndmsg {
	uint8_t ndm_family;
	uint8_t ndm_pad1;
	uint16_t ndm_pad2;
	int32_t ndm_ifindex;
	uint16_t ndm_state;
	uint8_t ndm_flags;
	uint8_t ndm_type;
};

enum {
	NDA_UNSPEC,
	NDA_DST,
	NDA_LLADDR,
	NDA_CACHEINFO,
	NDA_PROBES,
	NDA_VLAN,
	NDA_PORT,
	NDA_VNI,
	NDA_IFINDEX,
	NDA_MASTER,
	NDA_LINK_NETNSID,
	NDA_SRC_VNI,
	NDA_PROTOCOL,
	NDA_NH_ID,
	NDA_FDB_EXT_ATTRS,
	__NDA_MAX
};

#define NDA_MAX (__NDA_MAX - 1)

struct nda_cacheinfo {
	uint32_t ndm_confirmed;
	uint32_t ndm_used;
	uint32_t ndm_updated;
	uint32_t ndm_refcnt;
};

#ifdef __cplusplus
}
#endif

#endif

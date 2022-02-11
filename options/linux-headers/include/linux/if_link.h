#ifndef _LINUX_IF_LINK_H
#define _LINUX_IF_LINK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <stdint.h>

struct rtnl_link_stats {
	uint32_t rx_packets;
	uint32_t tx_packets;
	uint32_t rx_bytes;
	uint32_t tx_bytes;
	uint32_t rx_errors;
	uint32_t tx_errors;
	uint32_t rx_dropped;
	uint32_t tx_dropped;
	uint32_t multicast;
	uint32_t collisions;

	uint32_t rx_length_errors;
	uint32_t rx_over_errors;
	uint32_t rx_crc_errors;
	uint32_t rx_frame_errors;
	uint32_t rx_fifo_errors;
	uint32_t rx_missed_errors;

	uint32_t tx_aborted_errors;
	uint32_t tx_carrier_errors;
	uint32_t tx_fifo_errors;
	uint32_t tx_heartbeat_errors;
	uint32_t tx_window_errors;

	uint32_t rx_compressed;
	uint32_t tx_compressed;

	uint32_t rx_nohandler;
};

struct rtnl_link_stats64 {
	uint64_t rx_packets;
	uint64_t tx_packets;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	uint64_t rx_errors;
	uint64_t tx_errors;
	uint64_t rx_dropped;
	uint64_t tx_dropped;
	uint64_t multicast;
	uint64_t collisions;

	uint64_t rx_length_errors;
	uint64_t rx_over_errors;
	uint64_t rx_crc_errors;
	uint64_t rx_frame_errors;
	uint64_t rx_fifo_errors;
	uint64_t rx_missed_errors;

	uint64_t tx_aborted_errors;
	uint64_t tx_carrier_errors;
	uint64_t tx_fifo_errors;
	uint64_t tx_heartbeat_errors;
	uint64_t tx_window_errors;

	uint64_t rx_compressed;
	uint64_t tx_compressed;
	uint64_t rx_nohandler;
};

enum {
	IFLA_UNSPEC,
	IFLA_ADDRESS,
	IFLA_BROADCAST,
	IFLA_IFNAME,
	IFLA_MTU,
	IFLA_LINK,
	IFLA_QDISC,
	IFLA_STATS,
	IFLA_COST,
#define IFLA_COST IFLA_COST
	IFLA_PRIORITY,
#define IFLA_PRIORITY IFLA_PRIORITY
	IFLA_MASTER,
#define IFLA_MASTER IFLA_MASTER
	IFLA_WIRELESS,
#define IFLA_WIRELESS IFLA_WIRELESS
	IFLA_PROTINFO,
#define IFLA_PROTINFO IFLA_PROTINFO
	IFLA_TXQLEN,
#define IFLA_TXQLEN IFLA_TXQLEN
	IFLA_MAP,
#define IFLA_MAP IFLA_MAP
	IFLA_WEIGHT,
#define IFLA_WEIGHT IFLA_WEIGHT
	IFLA_OPERSTATE,
	IFLA_LINKMODE,
	IFLA_LINKINFO,
#define IFLA_LINKINFO IFLA_LINKINFO
	IFLA_NET_NS_PID,
	IFLA_IFALIAS,
	IFLA_NUM_VF,
	IFLA_VFINFO_LIST,
	IFLA_STATS64,
	IFLA_VF_PORTS,
	IFLA_PORT_SELF,
	IFLA_AF_SPEC,
	IFLA_GROUP,
	IFLA_NET_NS_FD,
	IFLA_EXT_MASK,
	IFLA_PROMISCUITY,
#define IFLA_PROMISCUITY IFLA_PROMISCUITY
	IFLA_NUM_TX_QUEUES,
	IFLA_NUM_RX_QUEUES,
	IFLA_CARRIER,
	IFLA_PHYS_PORT_ID,
	IFLA_CARRIER_CHANGES,
	IFLA_PHYS_SWITCH_ID,
	IFLA_LINK_NETNSID,
	IFLA_PHYS_PORT_NAME,
	IFLA_PROTO_DOWN,
	IFLA_GSO_MAX_SEGS,
	IFLA_GSO_MAX_SIZE,
	IFLA_PAD,
	IFLA_XDP,
	IFLA_EVENT,
	IFLA_NEW_NETNSID,
	IFLA_IF_NETNSID,
	IFLA_TARGET_NETNSID = IFLA_IF_NETNSID,
	IFLA_CARRIER_UP_COUNT,
	IFLA_CARRIER_DOWN_COUNT,
	IFLA_NEW_IFINDEX,
	IFLA_MIN_MTU,
	IFLA_MAX_MTU,
	IFLA_PROP_LIST,
	IFLA_ALT_IFNAME,
	IFLA_PERM_ADDRESS,
	IFLA_PROTO_DOWN_REASON,

	IFLA_PARENT_DEV_NAME,
	IFLA_PARENT_DEV_BUS_NAME,
	__IFLA_MAX
};

#define IFLA_MAX (__IFLA_MAX - 1)

enum {
	IFLA_VF_INFO_UNSPEC,
	IFLA_VF_INFO,
	__IFLA_VF_INFO_MAX,
};

#define IFLA_VF_INFO_MAX (__IFLA_VF_INFO_MAX - 1)

struct ifla_vf_mac {
	uint32_t vf;
	uint8_t mac[32];
};

enum {
    IFLA_VLAN_UNSPEC,
    IFLA_VLAN_ID,
    IFLA_VLAN_FLAGS,
    IFLA_VLAN_EGRESS_QOS,
    IFLA_VLAN_INGRESS_QOS,
    IFLA_VLAN_PROTOCOL,
    __IFLA_VLAN_MAX,
};

#define IFLA_VLAN_MAX (__IFLA_VLAN_MAX - 1)

struct ifla_vlan_flags {
    uint32_t flags;
    uint32_t mask;
};

enum {
    IFLA_VLAN_QOS_UNSPEC,
    IFLA_VLAN_QOS_MAPPING,
    __IFLA_VLAN_QOS_MAX
};

#define IFLA_VLAN_QOS_MAX (__IFLA_VLAN_QOS_MAX - 1)

struct ifla_vlan_qos_mapping {
    uint32_t from;
    uint32_t to;
};

enum {
	IFLA_VF_UNSPEC,
	IFLA_VF_MAC,
	IFLA_VF_VLAN,
	IFLA_VF_TX_RATE,
	IFLA_VF_SPOOFCHK,
	IFLA_VF_LINK_STATE,
	IFLA_VF_RATE,
	IFLA_VF_RSS_QUERY_EN,
	IFLA_VF_STATS,
	IFLA_VF_TRUST,
	IFLA_VF_IB_NODE_GUID,
	IFLA_VF_IB_PORT_GUID,
	IFLA_VF_VLAN_LIST,
	IFLA_VF_BROADCAST,
	__IFLA_VF_MAX,
};

#define IFLA_VF_MAX (__IFLA_VF_MAX - 1)

struct ifla_vf_rate {
	uint32_t vf;
	uint32_t min_tx_rate;
	uint32_t max_tx_rate;
};

struct ifla_vf_guid {
	uint32_t vf;
	uint64_t guid;
};

struct ifla_vf_link_state {
	uint32_t vf;
	uint32_t link_state;
};

struct ifla_vf_rss_query_en {
	uint32_t vf;
	uint32_t setting;
};

struct ifla_vf_trust {
	uint32_t vf;
	uint32_t setting;
};

#ifdef __cplusplus
}
#endif

#endif

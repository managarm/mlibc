#ifndef _LINUX_NETFILTER_NFNETLINK_H
#define _LINUX_NETFILTER_NFNETLINK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <linux/netfilter/nfnetlink_compat.h>
#include <stdint.h>

struct nfgenmsg {
	uint8_t nfgen_family;
	uint8_t version;
	uint16_t res_id;
};

#define NFNETLINK_V0 0

#define NFNL_SUBSYS_NFTABLES 10

#define NFNL_MSG_BATCH_BEGIN NLMSG_MIN_TYPE
#define NFNL_MSG_BATCH_END NLMSG_MIN_TYPE + 1

enum nfnl_batch_attributes {
	NFNL_BATCH_UNSPEC,
	NFNL_BATCH_GENID,
	__NFNL_BATCH_MAX
};
#define NFNL_BATCH_MAX (__NFNL_BATCH_MAX - 1)

#ifdef __cplusplus
}
#endif

#endif

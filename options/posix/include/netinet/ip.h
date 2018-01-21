
#ifndef _NETINET_IP_H
#define _NETINET_IP_H

#ifdef __cplusplus
extern "C" {
#endif

#define IPTOS_TOS_MASK 0x1E
#define IPTOS_TOS(tos) ((tos) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY 0x10
#define IPTOS_THROUGHPUT 0x08
#define IPTOS_RELIABILITY 0x04
#define IPTOS_LOWCOST 0x02
#define IPTOS_LOWDELAY IPTOS_LOWCOST

#ifdef __cplusplus
}
#endif

#endif // _NETINET_IP_H


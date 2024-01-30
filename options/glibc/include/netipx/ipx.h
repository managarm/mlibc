#ifndef _NETIPX_IPX_H
#define _NETIPX_IPX_H

#include <mlibc-config.h>
#include <sys/socket.h>
#include <stdint.h>

typedef struct ipx_config_data {
	unsigned char ipxcfg_auto_select_primary;
	unsigned char ipxcfg_auto_create_interfaces;
} ipx_config_data;

#define IPX_TYPE 1
#define IPX_NODE_LEN 6

struct sockaddr_ipx {
	sa_family_t sipx_family;
	uint16_t sipx_port;
	uint32_t sipx_network;
	unsigned char sipx_node[IPX_NODE_LEN];
	uint8_t sipx_type;
	unsigned char sipx_zero;
};

#define SOL_IPX 256

#if __MLIBC_LINUX_OPTION
#include <abi-bits/ioctls.h>

#define SIOCAIPXITFCRT (SIOCPROTOPRIVATE)
#define SIOCAIPXPRISLT (SIOCPROTOPRIVATE + 1)
#define SIOCIPXCFGDATA (SIOCPROTOPRIVATE + 2)
#endif

#endif /* _NETIPX_IPX_H */

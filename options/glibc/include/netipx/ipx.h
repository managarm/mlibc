#ifndef _NETIPX_IPX_H
#define _NETIPX_IPX_H

#include <mlibc-config.h>

typedef struct ipx_config_data {
	unsigned char ipxcfg_auto_select_primary;
	unsigned char ipxcfg_auto_create_interfaces;
} ipx_config_data;

#if __MLIBC_LINUX_OPTION
#include <abi-bits/ioctls.h>

#define SIOCAIPXITFCRT (SIOCPROTOPRIVATE)
#define SIOCAIPXPRISLT (SIOCPROTOPRIVATE + 1)
#define SIOCIPXCFGDATA (SIOCPROTOPRIVATE + 2)
#endif

#endif /* _NETIPX_IPX_H */

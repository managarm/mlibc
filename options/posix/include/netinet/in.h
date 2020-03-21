
#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <abi-bits/socket.h>
#include <abi-bits/in.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IN6_IS_ADDR_UNSPECIFIED 1
#define IN6_IS_ADDR_LOOPBACK(a) ({ \
    uint32_t *_a = (uint32_t *)((a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
    !_a[2] && \
     _a[3] == htonl(0x0001); \
})
#define IN6_IS_ADDR_MULTICAST 3
#define IN6_IS_ADDR_LINKLOCAL 4
#define IN6_IS_ADDR_SITELOCAL 5
#define IN6_IS_ADDR_V4MAPPED(a) ({ \
    uint32_t *_a = (uint32_t *)((a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
     _a[2] == htonl(0xffff); \
})
#define __ARE_4_BYTE_EQUAL(a, b) \
	((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && \
	 (a)[3] == (b)[3] && (a)[4] == (b)[4])
#define IN6_ARE_ADDR_EQUAL(a, b) \
	__ARE_4_BYTE_EQUAL((const uint32_t *)(a), (const uint32_t *)(b))

#define IN6_IS_ADDR_V4COMPAT 7
#define IN6_IS_ADDR_MC_NODELOCAL 8
#define IN6_IS_ADDR_MC_LINKLOCAL 9
#define IN6_IS_ADDR_MC_SITELOCAL 10
#define IN6_IS_ADDR_MC_ORGLOCAL 11
#define IN6_IS_ADDR_MC_GLOBAL 12

#ifdef __cplusplus
}
#endif

#endif // _NETINET_IN_H



#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <endian.h>
#include <sys/socket.h> // struct sockaddr
#include <abi-bits/socket.h>
#include <abi-bits/in.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t htonl(uint32_t);
uint16_t htons(uint16_t);
uint32_t ntohl(uint32_t);
uint16_t ntohs(uint16_t);

#define IN6_IS_ADDR_UNSPECIFIED(a) ({ \
    uint32_t *_a = (uint32_t *)((a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
    !_a[2] && \
    !_a[3];  \
})
#define IN6_IS_ADDR_LOOPBACK(a) ({ \
    uint32_t *_a = (uint32_t *)((a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
    !_a[2] && \
     _a[3] == htonl(0x0001); \
})
#define IN6_IS_ADDR_MULTICAST(a) (((const uint8_t *) (a))[0] == 0xff)
#define IN6_IS_ADDR_LINKLOCAL(a) ({ \
    uint32_t *_a = (uint32_t *)((a)->s6_addr); \
    _a[0] & htonl(0xffc00000) == htonl(0xfe800000); \
})
#define IN6_IS_ADDR_SITELOCAL(a) ({ \
    uint32_t *_a = (uint32_t *)((a)->s6_addr); \
    _a[0] & htonl(0xffc00000) == htonl(0xfec00000); \
})
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
#define IN6_IS_ADDR_MC_NODELOCAL(a) ({ \
    (IN6_IS_ADDR_MULTICAST(a) && \
    ((((const uint8_t *)(a))[1] & 0xf) == 0x1)); \
})
#define IN6_IS_ADDR_MC_LINKLOCAL(a) ({ \
    (IN6_IS_ADDR_MULTICAST(a) && \
    ((((const uint8_t *)(a))[1] & 0xf) == 0x2)); \
})
#define IN6_IS_ADDR_MC_SITELOCAL(a) ({ \
    (IN6_IS_ADDR_MULTICAST(a) && \
    ((((const uint8_t *)(a))[1] & 0xf) == 0x5)); \
})
#define IN6_IS_ADDR_MC_ORGLOCAL(a) ({ \
    (IN6_IS_ADDR_MULTICAST(a) && \
    ((((const uint8_t *)(a))[1] & 0xf) == 0x8)); \
})
#define IN6_IS_ADDR_MC_GLOBAL(a) ({ \
    (IN6_IS_ADDR_MULTICAST(a) && \
    ((((const uint8_t *)(a))[1] & 0xf) == 0xe)); \
})

#define IN_CLASSD(a) ((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(a) IN_CLASSD(a)

#define IN_CLASSA_NET 0xff000000
#define IN_CLASSB_NET 0xffff0000
#define IN_CLASSC_NET 0xffffff00

#ifdef __cplusplus
}
#endif

#endif // _NETINET_IN_H


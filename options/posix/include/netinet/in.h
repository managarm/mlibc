
#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <sys/socket.h> /* struct sockaddr */
#include <abi-bits/socket.h>
#include <abi-bits/in.h>
#include <arpa/inet.h>
#include <mlibc-config.h>

#if __MLIBC_GLIBC_OPTION
	#include <endian.h>
#endif /*__MLIBC_GLIBC_OPTION */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

uint32_t htonl(uint32_t __x);
uint16_t htons(uint16_t __x);
uint32_t ntohl(uint32_t __x);
uint16_t ntohs(uint16_t __x);

#endif /* !__MLIBC_ABI_ONLY */

#define IN6_IS_ADDR_UNSPECIFIED(a) ({ \
    uint32_t *_a = (uint32_t *)(((struct in6_addr *) a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
    !_a[2] && \
    !_a[3];  \
})
#define IN6_IS_ADDR_LOOPBACK(a) ({ \
    uint32_t *_a = (uint32_t *)(((struct in6_addr *) a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
    !_a[2] && \
     _a[3] == htonl(0x0001); \
})
#define IN6_IS_ADDR_MULTICAST(a) (((const uint8_t *) (a))[0] == 0xff)
#define IN6_IS_ADDR_LINKLOCAL(a) ({ \
    uint32_t *_a = (uint32_t *)(((struct in6_addr *) a)->s6_addr); \
    (_a[0] & htonl(0xffc00000)) == htonl(0xfe800000); \
})
#define IN6_IS_ADDR_SITELOCAL(a) ({ \
    uint32_t *_a = (uint32_t *)(((struct in6_addr *) a)->s6_addr); \
    (_a[0] & htonl(0xffc00000)) == htonl(0xfec00000); \
})
#define IN6_IS_ADDR_V4MAPPED(a) ({ \
    uint32_t *_a = (uint32_t *)(((struct in6_addr *) a)->s6_addr); \
    !_a[0] && \
    !_a[1] && \
     _a[2] == htonl(0xffff); \
})
#define __ARE_4_BYTE_EQUAL(a, b) \
	((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && \
	 (a)[3] == (b)[3])
#define IN6_ARE_ADDR_EQUAL(a, b) \
	__ARE_4_BYTE_EQUAL((const uint32_t *)(a), (const uint32_t *)(b))

#define IN6_IS_ADDR_V4COMPAT(a) ({ \
	uint32_t *_a = (uint32_t *)(((struct in6_addr *) a)->s6_addr); \
	uint8_t *_a8 = (uint8_t *)(((struct in6_addr *) a)->s6_addr); \
	!_a[0] && !_a[1] && !_a[2] && (_a8[15] > 1); \
})
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

#define IN_CLASSA(a) ((((in_addr_t)(a)) & 0x80000000) == 0)
#define IN_CLASSA_NET 0xff000000
#define IN_CLASSA_NSHIFT 24
#define IN_CLASSA_HOST (0xffffffff & ~IN_CLASSA_NET)
#define IN_CLASSA_MAX 128
#define IN_CLASSB(a) ((((in_addr_t)(a)) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET 0xffff0000
#define IN_CLASSB_NSHIFT 16
#define IN_CLASSB_HOST (0xffffffff & ~IN_CLASSB_NET)
#define IN_CLASSB_MAX 65536
#define IN_CLASSC(a) ((((in_addr_t)(a)) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET 0xffffff00
#define IN_CLASSC_NSHIFT 8
#define IN_CLASSC_HOST (0xffffffff & ~IN_CLASSC_NET)
#define IN_CLASSD(a) ((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(a) IN_CLASSD(a)
#define IN_EXPERIMENTAL(a) ((((in_addr_t)(a)) & 0xe0000000) == 0xe0000000)
#define IN_BADCLASS(a) ((((in_addr_t)(a)) & 0xf0000000) == 0xf0000000)

#define IN_LOOPBACKNET 127

#define MCAST_EXCLUDE 0
#define MCAST_INCLUDE 1

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_IN_H */


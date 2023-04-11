#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

uint32_t htonl(uint32_t);
uint16_t htons(uint16_t);
uint32_t ntohl(uint32_t);
uint16_t ntohs(uint16_t);

// ----------------------------------------------------------------------------
// IPv4 address manipulation.
// ----------------------------------------------------------------------------

in_addr_t inet_addr(const char *);
char *inet_ntoa(struct in_addr);

// GLIBC replacement for inet_addr().
int inet_aton(const char *, struct in_addr *);

// ----------------------------------------------------------------------------
// Generic IP address manipulation.
// ----------------------------------------------------------------------------
const char *inet_ntop(int, const void *__restrict, char *__restrict,
		socklen_t) __attribute__((__nonnull__(3)));
int inet_pton(int, const char *__restrict, void *__restrict);

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);
in_addr_t inet_netof(struct in_addr in);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _ARPA_INET_H


#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

uint32_t htonl(uint32_t __x);
uint16_t htons(uint16_t __x);
uint32_t ntohl(uint32_t __x);
uint16_t ntohs(uint16_t __x);

/* ---------------------------------------------------------------------------- */
/* IPv4 address manipulation. */
/* ---------------------------------------------------------------------------- */

in_addr_t inet_addr(const char *__cp);
in_addr_t inet_network(const char *__cp);
char *inet_ntoa(struct in_addr __in);

/* GLIBC replacement for inet_addr(). */
int inet_aton(const char *__cp, struct in_addr *__dest);

/* ---------------------------------------------------------------------------- */
/* Generic IP address manipulation. */
/* ---------------------------------------------------------------------------- */
const char *inet_ntop(int __af, const void *__restrict __src, char *__restrict __dst,
		socklen_t __size) __attribute__((__nonnull__(3)));
int inet_pton(int __af, const char *__restrict __src, void *__restrict __dst);

struct in_addr inet_makeaddr(in_addr_t __net, in_addr_t __host);
in_addr_t inet_netof(struct in_addr __in);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ARPA_INET_H */


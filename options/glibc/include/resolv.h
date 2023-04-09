#ifndef _RESOLV_H
#define _RESOLV_H

#include <netinet/in.h>

#define RES_INIT     0x00000001
#define RES_DEBUG    0x00000002
#define RES_USEVC    0x00000008
#define RES_IGNTC    0x00000020
#define RES_RECURSE  0x00000040
#define RES_DEFNAMES 0x00000080
#define RES_STAYOPEN 0x00000100
#define RES_DNSRCH   0x00000200

#define MAXNS     3
#define MAXDNSRCH 6

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int dn_expand(const unsigned char *, const unsigned char *,
		const unsigned char *, char *, int);

int res_query(const char *, int, int, unsigned char *, int);

int res_init(void);

#endif /* !__MLIBC_ABI_ONLY */

/* From musl: Unused; purely for broken apps
 * To avoid an massive struct, only add the items requested. */
typedef struct __res_state {
	int retrans;
	int retry;
	unsigned long options;
	int nscount;
	struct sockaddr_in nsaddr_list[MAXNS];
	char *dnsrch[MAXDNSRCH + 1];
	char defdname[256];
	unsigned ndots:4;
	unsigned nsort:4;
	union {
		char pad[52];
		struct {
			uint16_t nscount;
			uint16_t nsmap[MAXNS];
			int nssocks[MAXNS];
			uint16_t nscount6;
			uint16_t nsinit;
			struct sockaddr_in6	*nsaddrs[MAXNS];
			unsigned int _initstamp[2];
		} _ext;
	} _u;
} *res_state;

#ifndef __MLIBC_ABI_ONLY

struct __res_state *__res_state(void);
#define _res (*__res_state())

int res_ninit(res_state);
void res_nclose(res_state);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _RESOLV_H

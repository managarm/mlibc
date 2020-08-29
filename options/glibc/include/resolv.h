#ifndef _RESOLV_H
#define _RESOLV_H

#define RES_INIT     0x00000001
#define RES_DEBUG    0x00000002
#define RES_USEVC    0x00000008
#define RES_IGNTC    0x00000020
#define RES_RECURSE  0x00000040
#define RES_DEFNAMES 0x00000080
#define RES_STAYOPEN 0x00000100
#define RES_DNSRCH   0x00000200

#ifdef __cplusplus
extern "C" {
#endif

int dn_expand(const unsigned char *, const unsigned char *,
		const unsigned char *, char *, int);

int res_query(const char *, int, int, unsigned char *, int);

int res_init(void);

/* From musl: Unused; purely for broken apps
 * To avoid an massive struct, only add the items requested. */
typedef struct __res_state {
	unsigned long options;
} *res_state;
struct __res_state *__res_state(void);
#define _res (*__res_state())

#ifdef __cplusplus
}
#endif

#endif // _RESOLV_H

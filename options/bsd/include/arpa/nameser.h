#ifndef _ARPA_NAMESER_H
#define _ARPA_NAMESER_H

#include <stdint.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NS_PACKETSZ 512
#define NS_MAXDNAME 1025
#define NS_MAXLABEL 63

typedef	enum __ns_rcode {
	ns_r_noerror = 0,
	ns_r_formerr = 1,
	ns_r_servfail = 2,
	ns_r_nxdomain = 3,
	ns_r_notimpl = 4,
	ns_r_refused = 5,
	ns_r_yxdomain = 6,
	ns_r_yxrrset = 7,
	ns_r_nxrrset = 8,
	ns_r_notauth = 9,
	ns_r_notzone = 10,
	ns_r_max = 11,
	ns_r_badvers = 16,
	ns_r_badsig = 16,
	ns_r_badkey = 17,
	ns_r_badtime = 18
} ns_rcode;

typedef enum __ns_type {
	ns_t_invalid = 0,
	ns_t_a = 1,
	ns_t_ns = 2,
	ns_t_md = 3,
	ns_t_mf = 4,
	ns_t_cname = 5,
	ns_t_soa = 6,
	ns_t_mb = 7,
	ns_t_mg = 8,
	ns_t_mr = 9,
	ns_t_null = 10,
	ns_t_wks = 11,
	ns_t_ptr = 12,
	ns_t_hinfo = 13,
	ns_t_minfo = 14,
	ns_t_mx = 15,
	ns_t_txt = 16,
	ns_t_rp = 17,
	ns_t_afsdb = 18,
	ns_t_x25 = 19,
	ns_t_isdn = 20,
	ns_t_rt = 21,
	ns_t_nsap = 22,
	ns_t_nsap_ptr = 23,
	ns_t_sig = 24,
	ns_t_key = 25,
	ns_t_px = 26,
	ns_t_gpos = 27,
	ns_t_aaaa = 28,
	ns_t_loc = 29,
	ns_t_nxt = 30,
	ns_t_eid = 31,
	ns_t_nimloc = 32,
	ns_t_srv = 33,
	ns_t_atma = 34,
	ns_t_naptr = 35,
	ns_t_kx = 36,
	ns_t_cert = 37,
	ns_t_a6 = 38,
	ns_t_dname = 39,
	ns_t_sink = 40,
	ns_t_opt = 41,
	ns_t_apl = 42,
	ns_t_tkey = 249,
	ns_t_tsig = 250,
	ns_t_ixfr = 251,
	ns_t_axfr = 252,
	ns_t_mailb = 253,
	ns_t_maila = 254,
	ns_t_any = 255,
	ns_t_zxfr = 256,
	ns_t_max = 65536
} ns_type;

typedef enum __ns_class {
	ns_c_invalid = 0,
	ns_c_in = 1,
	ns_c_2 = 2,
	ns_c_chaos = 3,
	ns_c_hs = 4,
	ns_c_none = 254,
	ns_c_any = 255,
	ns_c_max = 65536
} ns_class;

typedef enum __ns_sect {
	ns_s_qd = 0,
	ns_s_zn = 0,
	ns_s_an = 1,
	ns_s_pr = 1,
	ns_s_ns = 2,
	ns_s_ud = 2,
	ns_s_ar = 3,
	ns_s_max = 4
} ns_sect;

typedef struct __ns_msg {
	const unsigned char	*_msg, *_eom;
	uint16_t _id, _flags, _counts[ns_s_max];
	const unsigned char	*_sections[ns_s_max];
	ns_sect _sect;
	int _rrnum;
	const unsigned char	*_msg_ptr;
} ns_msg;

#define ns_msg_id(handle) ((handle)._id + 0)
#define ns_msg_base(handle) ((handle)._msg + 0)
#define ns_msg_end(handle) ((handle)._eom + 0)
#define ns_msg_size(handle) ((handle)._eom - (handle)._msg)
#define ns_msg_count(handle, section) ((handle)._counts[section] + 0)

typedef	struct __ns_rr {
	char name[NS_MAXDNAME];
	uint16_t type;
	uint16_t rr_class;
	uint32_t ttl;
	uint16_t rdlength;
	const unsigned char *rdata;
} ns_rr;

#define ns_rr_name(rr)	(((rr).name[0] != '\0') ? (rr).name : ".")
#define ns_rr_type(rr)	((ns_type)((rr).type + 0))
#define ns_rr_class(rr)	((ns_class)((rr).rr_class + 0))
#define ns_rr_ttl(rr)	((rr).ttl + 0)
#define ns_rr_rdlen(rr)	((rr).rdlength + 0)
#define ns_rr_rdata(rr)	((rr).rdata + 0)

#ifndef __MLIBC_ABI_ONLY

#define NS_GET16(s, cp) (void)((s) = ns_get16(((cp) += 2) - 2))
#define NS_GET32(l, cp) (void)((l) = ns_get32(((cp) += 4) - 4))
#define NS_PUT16(s, cp) ns_put16((s), ((cp) += 2) - 2)
#define NS_PUT32(l, cp) ns_put32((l), ((cp) += 4) - 4)

unsigned ns_get16(const unsigned char *__src);
unsigned long ns_get32(const unsigned char *__src);
void ns_put16(unsigned int __value, unsigned char *__src);
void ns_put32(unsigned long __value, unsigned char *__src);

int ns_initparse(const unsigned char *__msg, int __msglen, ns_msg *__handle);
int ns_parserr(ns_msg *__msg, ns_sect __section, int __rrnum, ns_rr *__rr);
int ns_name_uncompress(const unsigned char *__msg, const unsigned char *__eom,
				    const unsigned char *__src, char *__dst, size_t __dstsize);

#endif /* !__MLIBC_ABI_ONLY */

typedef struct {
	unsigned	id :16;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned	qr: 1;
	unsigned	opcode: 4;
	unsigned	aa: 1;
	unsigned	tc: 1;
	unsigned	rd: 1;
	unsigned	ra: 1;
	unsigned	unused :1;
	unsigned	ad: 1;
	unsigned	cd: 1;
	unsigned	rcode :4;
#else
	unsigned	rd :1;
	unsigned	tc :1;
	unsigned	aa :1;
	unsigned	opcode :4;
	unsigned	qr :1;
	unsigned	rcode :4;
	unsigned	cd: 1;
	unsigned	ad: 1;
	unsigned	unused :1;
	unsigned	ra :1;
#endif
	unsigned	qdcount :16;
	unsigned	ancount :16;
	unsigned	nscount :16;
	unsigned	arcount :16;
} HEADER;

#define PACKETSZ	NS_PACKETSZ
#define MAXDNAME	NS_MAXDNAME

#define NOERROR		ns_r_noerror
#define FORMERR		ns_r_formerr
#define SERVFAIL	ns_r_servfail
#define NXDOMAIN	ns_r_nxdomain
#define NOTIMP		ns_r_notimpl
#define REFUSED		ns_r_refused
#define YXDOMAIN	ns_r_yxdomain
#define YXRRSET		ns_r_yxrrset
#define NXRRSET		ns_r_nxrrset
#define NOTAUTH		ns_r_notauth
#define NOTZONE		ns_r_notzone

#define T_A			ns_t_a
#define T_NS		ns_t_ns
#define T_MD		ns_t_md
#define T_MF		ns_t_mf
#define T_CNAME		ns_t_cname
#define T_SOA		ns_t_soa
#define T_MB		ns_t_mb
#define T_MG		ns_t_mg
#define T_MR		ns_t_mr
#define T_NULL		ns_t_null
#define T_WKS		ns_t_wks
#define T_PTR		ns_t_ptr
#define T_HINFO		ns_t_hinfo
#define T_MINFO		ns_t_minfo
#define T_MX		ns_t_mx
#define T_TXT		ns_t_txt
#define	T_RP		ns_t_rp
#define T_AFSDB		ns_t_afsdb
#define T_X25		ns_t_x25
#define T_ISDN		ns_t_isdn
#define T_RT		ns_t_rt
#define T_NSAP		ns_t_nsap
#define T_NSAP_PTR	ns_t_nsap_ptr
#define	T_SIG		ns_t_sig
#define	T_KEY		ns_t_key
#define	T_PX		ns_t_px
#define	T_GPOS		ns_t_gpos
#define	T_AAAA		ns_t_aaaa
#define	T_LOC		ns_t_loc
#define	T_NXT		ns_t_nxt
#define	T_EID		ns_t_eid
#define	T_NIMLOC	ns_t_nimloc
#define	T_SRV		ns_t_srv
#define T_ATMA		ns_t_atma
#define T_NAPTR		ns_t_naptr
#define T_A6		ns_t_a6
#define T_DNAME		ns_t_dname
#define	T_TSIG		ns_t_tsig
#define	T_IXFR		ns_t_ixfr
#define T_AXFR		ns_t_axfr
#define T_MAILB		ns_t_mailb
#define T_MAILA		ns_t_maila
#define T_ANY		ns_t_any

#define C_IN		ns_c_in
#define C_CHAOS		ns_c_chaos
#define C_HS		ns_c_hs
#define C_NONE		ns_c_none
#define C_ANY		ns_c_any

#define	GETSHORT	NS_GET16
#define	GETLONG		NS_GET32
#define	PUTSHORT	NS_PUT16
#define	PUTLONG		NS_PUT32

#ifdef __cplusplus
}
#endif

#endif /* _ARPA_NAMESER_H */

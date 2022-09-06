#ifndef _NETAX25_AX25_H
#define _NETAX25_AX25_H

#include <mlibc-config.h>
#include <sys/socket.h>

#define AX25_VALUES_IPDEFMODE   0
#define AX25_VALUES_AXDEFMODE   1
#define AX25_VALUES_NETROM      2
#define AX25_VALUES_TEXT        3
#define AX25_VALUES_BACKOFF     4
#define AX25_VALUES_CONMODE     5
#define AX25_VALUES_WINDOW      6
#define AX25_VALUES_EWINDOW     7
#define AX25_VALUES_T1          8
#define AX25_VALUES_T2          9
#define AX25_VALUES_T3          10
#define AX25_VALUES_N2          11
#define AX25_VALUES_DIGI        12
#define AX25_VALUES_IDLE        13
#define AX25_VALUES_PACLEN      14
#define AX25_VALUES_IPMAXQUEUE  15
#define AX25_MAX_VALUES         20

typedef struct {
	char ax25_call[7];
} ax25_address;

struct sockaddr_ax25 {
	sa_family_t sax25_family;
	ax25_address sax25_call;
	int sax25_ndigis;
};

struct ax25_parms_struct {
	ax25_address port_addr;
	unsigned short values[AX25_MAX_VALUES];
};

#if __MLIBC_LINUX_OPTION
#include <abi-bits/ioctls.h>

#define SIOCAX25GETUID (SIOCPROTOPRIVATE)
#define SIOCAX25ADDUID (SIOCPROTOPRIVATE + 1)
#define SIOCAX25DELUID (SIOCPROTOPRIVATE + 2)
#define SIOCAX25NOUID (SIOCPROTOPRIVATE + 3)
#define SIOCAX25GETPARMS (SIOCPROTOPRIVATE + 5)
#define SIOCAX25SETPARMS (SIOCPROTOPRIVATE + 6)
#endif /* __MLIBC_LINUX_OPTION */

#endif /* _NETAX25_AX25_H */

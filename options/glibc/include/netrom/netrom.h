#ifndef _NETROM_NETROM_H
#define _NETROM_NETROM_H

#include <mlibc-config.h>

struct nr_parms_struct {
	unsigned int quality;
	unsigned int obs_count;
	unsigned int ttl;
	unsigned int timeout;
	unsigned int ack_delay;
	unsigned int busy_delay;
	unsigned int tries;
	unsigned int window;
	unsigned int paclen;
};

#if __MLIBC_LINUX_OPTION
#include <abi-bits/ioctls.h>

#define SIOCNRGETPARMS (SIOCPROTOPRIVATE)
#define SIOCNRSETPARMS (SIOCPROTOPRIVATE + 1)
#define SIOCNRDECOBS (SIOCPROTOPRIVATE + 2)
#define SIOCNRRTCTL (SIOCPROTOPRIVATE + 3)
#endif

#endif /* _NETROM_NETROM_H */

#ifndef _MLIBC_SERVICES
#define _MLIBC_SERVICES

namespace mlibc {

// Only two services for tcp and udp
#define SERV_MAX 2

struct service_buf {
	// That's all we need for now, for getservbyname we
	// will also need the name...
	int port, protocol, socktype;
};

int lookup_serv(struct service_buf *buf, const char *name, int proto,
		int socktype, int flags);

} // namespace mlibc

#endif // _MLIBC_SERVICES

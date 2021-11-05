#ifndef _ABIBITS_SOCKET_H
#define _ABIBITS_SOCKET_H

#include <bits/posix/socklen_t.h>
#include <bits/posix/iovec.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int sa_family_t;

struct msghdr {
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	int msg_iovlen;
	void *msg_control;
	socklen_t msg_controllen;
	int msg_flags;
};

struct sockaddr_storage {
	sa_family_t ss_family;
	char __padding[128 - sizeof(sa_family_t)];
};

struct mmsghdr {
	struct msghdr msg_hdr;
	unsigned int  msg_len;
};

#ifdef __cplusplus
}
#endif

#define SCM_RIGHTS 1

//MISSING: CMSG_DATA, CMSG_NXTHDR, CMSG_FIRSTHDR

#define SCM_CREDENTIALS 0x02

#define SOCK_DGRAM 1
#define SOCK_RAW 2
#define SOCK_SEQPACKET 3
#define SOCK_STREAM 4
#define SOCK_NONBLOCK 0x10000
#define SOCK_CLOEXEC 0x20000

#define SOL_SOCKET 1

#define SO_ACCEPTCONN 1
#define SO_BROADCAST 2
#define SO_DEBUG 3
#define SO_DONTROUTE 4
#define SO_ERROR 5
#define SO_KEEPALIVE 6
#define SO_LINGER 7
#define SO_OOBINLINE 8
#define SO_RCVBUF 9
#define SO_RCVLOWAT 10
#define SO_RCVTIMEO 11
#define SO_REUSEADDR 12
#define SO_SNDBUF 13
#define SO_SNDLOWAT 14
#define SO_SNDTIMEO 15
#define SO_TYPE 16
#define SO_SNDBUFFORCE 17
#define SO_PEERCRED 18
#define SO_ATTACH_FILTER 19
#define SO_PASSCRED 20
#define SO_RCVBUFFORCE 21
#define SO_DETACH_FILTER 22
#define SO_PROTOCOL 23

#define SOMAXCONN 1

#define MSG_CTRUNC 0x1
#define MSG_DONTROUTE 0x2
#define MSG_EOR 0x4
#define MSG_OOB 0x8
#define MSG_NOSIGNAL 0x10
#define MSG_PEEK 0x20
#define MSG_TRUNC 0x40
#define MSG_WAITALL 0x80

// Linux extensions.
#define MSG_DONTWAIT 0x1000
#define MSG_CMSG_CLOEXEC 0x2000

// GNU (?) extension: Protocol family constants.

#define PF_INET 1
#define PF_INET6 2
#define PF_UNIX 3
#define PF_LOCAL 3
#define PF_UNSPEC 4
#define PF_NETLINK 5
#define PF_BRIDGE 6

#define AF_INET PF_INET
#define AF_INET6 PF_INET6
#define AF_UNIX PF_UNIX
#define AF_LOCAL PF_LOCAL
#define AF_UNSPEC PF_UNSPEC
#define AF_NETLINK PF_NETLINK
#define AF_BRIDGE PF_BRIDGE

#define SHUT_RD 1
#define SHUT_RDWR 2
#define SHUT_WR 3
#endif

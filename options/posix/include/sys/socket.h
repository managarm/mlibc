
#ifndef _SOCKET_H
#define _SOCKET_H

#include <bits/posix/gid_t.h>
#include <bits/posix/pid_t.h>
#include <bits/posix/sa_family_t.h>
#include <bits/size_t.h>
#include <bits/posix/socklen_t.h>
#include <bits/posix/ssize_t.h>
#include <bits/posix/uid_t.h>
#include <bits/posix/iovec.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[];
};

//MISSING: struct sockaddr_storage

struct msghdr {
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	int msg_iovlen;
	void *msg_control;
	socklen_t msg_controllen;
	int msg_flags;
};

struct cmsghdr {
	socklen_t cmsg_len;
	int cmsg_level;
	int cmsg_type;
};

#define CMSG_LEN(sz) (sizeof(struct cmsghdr) + (sz))
#define CMSG_ALIGN(sz) (((sz) + __alignof__(struct cmsghdr) - 1) & \
		~(__alignof__(struct cmsghdr) - 1))
#define CMSG_SPACE(sz) (sizeof(struct cmsghdr) + CMSG_ALIGN(sz))

#define __MLIBC_CMSG_NEXT(cmsg) ((char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len))
#define __MLIBC_MHDR_LIMIT(mhdr) ((char *)(mhdr)->msg_control + (mhdr)->msg_controllen)

#define CMSG_DATA(cmsg) ((char *)(((struct cmsghdr *)(cmsg)) + 1))
#define CMSG_NXTHDR(mhdr, cmsg) \
	((cmsg)->cmsg_len < sizeof(struct cmsghdr) || \
		sizeof(struct cmsghdr) + CMSG_ALIGN((cmsg)->cmsg_len) \
			>= __MLIBC_MHDR_LIMIT(mhdr) - (char *)(cmsg) \
	? (struct cmsghdr *)0 : (struct cmsghdr *)__MLIBC_CMSG_NEXT(cmsg))
#define CMSG_FIRSTHDR(mhdr) ((size_t)(mhdr)->msg_controllen <= sizeof(struct cmsghdr) \
	? (struct cmsghdr *)0 : (struct cmsghdr *) (mhdr)->msg_control)

#define SCM_RIGHTS 1

//MISSING: CMSG_DATA, CMSG_NXTHDR, CMSG_FIRSTHDR

struct linger{
	int l_onoff;
	int l_linger;
};

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

#define AF_INET PF_INET
#define AF_INET6 PF_INET6
#define AF_UNIX PF_UNIX
#define AF_LOCAL PF_LOCAL
#define AF_UNSPEC PF_UNSPEC
#define AF_NETLINK PF_NETLINK

#define SHUT_RD 1
#define SHUT_RDWR 2
#define SHUT_WR 3

struct ucred {
	pid_t pid;
	uid_t uid;
	gid_t gid;
};

int accept(int, struct sockaddr *__restrict, socklen_t *__restrict);
int bind(int, const struct sockaddr *, socklen_t);
int connect(int, const struct sockaddr *, socklen_t);
int getpeername(int, struct sockaddr *__restrict, socklen_t *__restrict);
int getsockname(int, struct sockaddr *__restrict, socklen_t *__restrict);
int getsockopt(int, int, int, void *__restrict, socklen_t *__restrict);
int listen(int, int);
ssize_t recv(int, void *, size_t, int);
ssize_t recvfrom(int, void *__restrict, size_t, int, struct sockaddr *__restrict, socklen_t *__restrict);
ssize_t recvmsg(int, struct msghdr *, int);
ssize_t send(int, const void *, size_t, int);
ssize_t sendmsg(int, const struct msghdr *, int);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
int setsockopt(int, int, int, const void *, socklen_t);
int shutdown(int, int);
int sockatmark(int);
int socket(int, int, int);
int socketpair(int, int, int, int [2]);

#ifdef __cplusplus
}
#endif

#endif // _UNISTD_H


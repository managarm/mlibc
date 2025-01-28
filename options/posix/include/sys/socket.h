
#ifndef _SOCKET_H
#define _SOCKET_H

#include <abi-bits/gid_t.h>
#include <abi-bits/pid_t.h>
#include <bits/size_t.h>
#include <abi-bits/socklen_t.h>
#include <bits/ssize_t.h>
#include <abi-bits/uid_t.h>
#include <bits/posix/iovec.h>
#include <abi-bits/socket.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
};

/* Control message format: */
/* The offsets marked with ^ are aligned to alignof(size_t). */
/* */
/* |---HEADER---|---DATA---|---PADDING---|---HEADER---|... */
/* ^            ^                        ^ */
/* |---------CMSG_LEN------| */
/* |---------------CMSG_SPACE------------| */

/* Auxiliary macro. While there is basically no reason for applications */
/* to use this, it is exported by glibc. */
#define CMSG_ALIGN(s) (((s) + __alignof__(size_t) - 1) & \
		~(__alignof__(size_t) - 1))

/* Basic macros to return content and padding size of a control message. */
#define CMSG_LEN(s) (CMSG_ALIGN(sizeof(struct cmsghdr)) + (s))
#define CMSG_SPACE(s) (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(s))

/* Provides access to the data of a control message. */
#define CMSG_DATA(c) ((char *)(c) + CMSG_ALIGN(sizeof(struct cmsghdr)))

#define __MLIBC_CMSG_NEXT(c) ((char *)(c) + CMSG_ALIGN((c)->cmsg_len))
#define __MLIBC_MHDR_LIMIT(m) ((char *)(m)->msg_control + (m)->msg_controllen)

/* For parsing control messages only. */
/* Returns a pointer to the first header or nullptr if there is none. */
#define CMSG_FIRSTHDR(m) ((size_t)(m)->msg_controllen <= sizeof(struct cmsghdr) \
	? (struct cmsghdr *)0 : (struct cmsghdr *) (m)->msg_control)

/* For parsing control messages only. */
/* Returns a pointer to the next header or nullptr if there is none. */
#define CMSG_NXTHDR(m, c) \
	((c)->cmsg_len < sizeof(struct cmsghdr) || \
		(ptrdiff_t)(sizeof(struct cmsghdr) + CMSG_ALIGN((c)->cmsg_len)) \
			>= __MLIBC_MHDR_LIMIT(m) - (char *)(c) \
	? (struct cmsghdr *)0 : (struct cmsghdr *)__MLIBC_CMSG_NEXT(c))

struct linger{
	int l_onoff;
	int l_linger;
};

struct ucred {
	pid_t pid;
	uid_t uid;
	gid_t gid;
};

#ifndef __MLIBC_ABI_ONLY

int accept(int __sockfd, struct sockaddr *__restrict __addr, socklen_t *__restrict __addrlen);
int accept4(int __sockfd, struct sockaddr *__restrict __addr, socklen_t *__restrict __addrlen, int __flags);
int bind(int __sockfd, const struct sockaddr *__addr, socklen_t __addrlen);
int connect(int __sockfd, const struct sockaddr *__addr, socklen_t __addrlen);
int getpeername(int __sockfd, struct sockaddr *__restrict __addr, socklen_t *__restrict __addrlen);
int getsockname(int __sockfd, struct sockaddr *__restrict __addr, socklen_t *__restrict __addrlen);
int getsockopt(int __sockfd, int __level, int __optname, void *__restrict __optval, socklen_t *__restrict __optlen);
int listen(int __sockfd, int __backlog);
ssize_t recv(int __sockfd, void *__buf, size_t __size, int __flags);
ssize_t recvfrom(int __sockfd, void *__restrict __buf, size_t __size, int __flags,
		struct sockaddr *__restrict __src_addr, socklen_t *__restrict __addrlen);
ssize_t recvmsg(int __sockfd, struct msghdr *__msg, int __flags);
ssize_t send(int __sockfd, const void *__buf, size_t __size, int __flags);
ssize_t sendmsg(int __sockfd, const struct msghdr *__msg, int __flags);
ssize_t sendto(int __sockfd, const void *__buf, size_t __size, int __flags,
		const struct sockaddr *__dest_addr, socklen_t __addrlen);
int recvmmsg(int __sockfd, struct mmsghdr *__msgvec, unsigned int __vlen, int __flags, struct timespec *__timeout);
int sendmmsg(int __sockfd, struct mmsghdr *__msgvec, unsigned int __vlen, int __flags);
int setsockopt(int __sockfd, int __level, int __option_name, const void *__optval, socklen_t __optlen);
int shutdown(int __sockfd, int __how);
int sockatmark(int __sockfd);
int socket(int __domain, int __type, int __protocol);
int socketpair(int __domain, int __type, int __protocol, int __sv[2]);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H */


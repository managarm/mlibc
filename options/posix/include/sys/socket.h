
#ifndef _SOCKET_H
#define _SOCKET_H

#include <abi-bits/gid_t.h>
#include <abi-bits/pid_t.h>
#include <bits/size_t.h>
#include <bits/posix/socklen_t.h>
#include <bits/ssize_t.h>
#include <abi-bits/uid_t.h>
#include <bits/posix/iovec.h>
#include <abi-bits/socket.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
};

struct cmsghdr {
	socklen_t cmsg_len;
	int cmsg_level;
	int cmsg_type;
};

// Control message format:
// The offsets marked with ^ are aligned to alignof(size_t).
//
// |---HEADER---|---DATA---|---PADDING---|---HEADER---|...
// ^            ^                        ^
// |---------CMSG_LEN------|
// |---------------CMSG_SPACE------------|

// Auxiliary macro. While there is basically no reason for applications
// to use this, it is exported by glibc.
#define CMSG_ALIGN(s) (((s) + __alignof__(size_t) - 1) & \
		~(__alignof__(size_t) - 1))

// Basic macros to return content and padding size of a control message.
#define CMSG_LEN(s) (sizeof(struct cmsghdr) + (s))
#define CMSG_SPACE(s) (sizeof(struct cmsghdr) + CMSG_ALIGN(s))

// Provides access to the data of a control message.
#define CMSG_DATA(c) ((char *)(c) + sizeof(struct cmsghdr))

#define __MLIBC_CMSG_NEXT(c) ((char *)(c) + CMSG_ALIGN((c)->cmsg_len))
#define __MLIBC_MHDR_LIMIT(m) ((char *)(m)->msg_control + (m)->msg_controllen)

// For parsing control messages only.
// Returns a pointer to the first header or nullptr if there is none.
#define CMSG_FIRSTHDR(m) ((size_t)(m)->msg_controllen <= sizeof(struct cmsghdr) \
	? (struct cmsghdr *)0 : (struct cmsghdr *) (m)->msg_control)

// For parsing control messages only.
// Returns a pointer to the next header or nullptr if there is none.
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
int recvmmsg(int sockfd, struct mmsghdr *msgvec, unsigned int vlen, int flags, struct timespec *timeout);
int sendmmsg(int sockfd, struct mmsghdr *msgvec, unsigned int vlen, int flags);
int setsockopt(int, int, int, const void *, socklen_t);
int shutdown(int, int);
int sockatmark(int);
int socket(int, int, int);
int socketpair(int, int, int, int [2]);

#ifdef __cplusplus
}
#endif

#endif // _UNISTD_H


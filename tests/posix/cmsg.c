#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

int main() {
	assert(CMSG_ALIGN(1) == sizeof(size_t));
	assert(CMSG_ALIGN(sizeof(size_t)) == sizeof(size_t));
	assert(CMSG_ALIGN(sizeof(size_t) + 1) == sizeof(size_t) * 2);

	assert(CMSG_LEN(0) == CMSG_ALIGN(sizeof(struct cmsghdr)));
	assert(CMSG_SPACE(0) == CMSG_ALIGN(sizeof(struct cmsghdr)));
	assert(CMSG_SPACE(1) == CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(1));

	struct msghdr msg;
	union {
		char buf[128];
		struct cmsghdr align;
	} u;
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = u.buf;

	// check control size == 0
	msg.msg_controllen = 0;
	assert(CMSG_FIRSTHDR(&msg) == NULL);

	// check control size < sizeof(struct cmsghdr)
	msg.msg_controllen = sizeof(struct cmsghdr) - 1;
	assert(CMSG_FIRSTHDR(&msg) == NULL);

	// check control size == sizeof(struct cmsghdr)
	msg.msg_controllen = sizeof(struct cmsghdr);
	assert(CMSG_FIRSTHDR(&msg) == (struct cmsghdr *)u.buf);

	// check control size > sizeof(struct cmsghdr)
	msg.msg_controllen = sizeof(struct cmsghdr) + 1;
	assert(CMSG_FIRSTHDR(&msg) == (struct cmsghdr *)u.buf);

	struct cmsghdr *cmsg1 = (struct cmsghdr *)u.buf;
	// payload of 8 bytes
	cmsg1->cmsg_len = sizeof(struct cmsghdr) + 8;

	// we want the second header to fit exactly.
	// the second header starts at CMSG_ALIGN(cmsg1->cmsg_len)
	// the size of the second header is sizeof(struct cmsghdr)
	size_t space_needed = CMSG_ALIGN(cmsg1->cmsg_len) + sizeof(struct cmsghdr);
	msg.msg_controllen = space_needed;

	// CMSG_NXTHDR should return the pointer to the next header.
	struct cmsghdr *cmsg2 = CMSG_NXTHDR(&msg, cmsg1);
	assert(cmsg2 != NULL);
	assert((char *)cmsg2 == u.buf + CMSG_ALIGN(cmsg1->cmsg_len));

	// if msg_controllen == (space_needed - 1), the second header shouldn't fit
	msg.msg_controllen = space_needed - 1;
	assert(CMSG_NXTHDR(&msg, cmsg1) == NULL);

	return 0;
}

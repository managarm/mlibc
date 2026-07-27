#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

int main() {
	struct msghdr msg;
	char buf[128];
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = buf;

	// check control size == 0
	msg.msg_controllen = 0;
	assert(CMSG_FIRSTHDR(&msg) == NULL);

	// check control size < sizeof(struct cmsghdr)
	msg.msg_controllen = sizeof(struct cmsghdr) - 1;
	assert(CMSG_FIRSTHDR(&msg) == NULL);

	// check control size == sizeof(struct cmsghdr)
	msg.msg_controllen = sizeof(struct cmsghdr);
	assert(CMSG_FIRSTHDR(&msg) == (struct cmsghdr *)buf);

	// check control size > sizeof(struct cmsghdr)
	msg.msg_controllen = sizeof(struct cmsghdr) + 1;
	assert(CMSG_FIRSTHDR(&msg) == (struct cmsghdr *)buf);

	struct cmsghdr *cmsg1 = (struct cmsghdr *)buf;
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
	assert((char *)cmsg2 == buf + CMSG_ALIGN(cmsg1->cmsg_len));

	// if msg_controllen == (space_needed - 1), the second header shouldn't fit
	msg.msg_controllen = space_needed - 1;
	assert(CMSG_NXTHDR(&msg, cmsg1) == NULL);

	return 0;
}

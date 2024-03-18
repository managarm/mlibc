#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#ifdef USE_HOST_LIBC
#define TEST_PORT 31337
#else
#define TEST_PORT 42069
#endif

static struct sockaddr_in connect_addr, accept_addr;
static int listen_fd;

int permutations[] = {
	0,
	SOCK_CLOEXEC,
	SOCK_NONBLOCK,
	SOCK_CLOEXEC | SOCK_NONBLOCK,
};

static bool run_test(int flags)
{
	int connect_fd;
	int fd_flags, access;
	socklen_t addrlen;

	connect_fd = socket(AF_INET, SOCK_STREAM, 0);
	connect(connect_fd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));
	addrlen = sizeof(accept_addr);

	int accept_fd = accept4(listen_fd, (struct sockaddr *)&accept_addr, &addrlen, flags);

	if(accept_fd == -1) {
		fprintf(stderr, "accept4 failed: %s\n", strerror(errno));
		goto cleanup;
	}

	fd_flags = fcntl(accept_fd, F_GETFD);
	if(!(!!(fd_flags & FD_CLOEXEC) == !!(flags & SOCK_CLOEXEC))) {
		fprintf(stderr, "CLOEXEC mismatch, got %d instead of %d\n", !!(fd_flags & FD_CLOEXEC), !!(flags & SOCK_CLOEXEC));
		goto cleanup;
	}

	access = fcntl(accept_fd, F_GETFL);
	if(!(!!(access & O_NONBLOCK) == !!(flags & SOCK_NONBLOCK))) {
		fprintf(stderr, "NONBLOCK flag mismatch, %d vs %d\n", !!(access & O_NONBLOCK), !!(flags & SOCK_NONBLOCK));
		goto cleanup;
	}

	close(accept_fd);
	close(connect_fd);

	fprintf(stderr, "tested CLOEXEC %d, NONBLOCK %d\n", !!(flags & SOCK_CLOEXEC), !!(flags & SOCK_NONBLOCK));
	return true;

cleanup:
	close(accept_fd);
	close(connect_fd);
	return false;
}

static int socket_setup(void)
{
	struct sockaddr_in addr;
	int reuseaddr;

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(TEST_PORT);

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_fd == -1) {
		fprintf(stderr, "socket failed: %s\n", strerror(errno));
		exit(1);
	}

	reuseaddr = 1;
	int ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

	if(ret == -1) {
		fprintf(stderr, "setsockopt failed: %s\n", strerror(errno));
		exit(1);
	}

	ret = bind(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

	if(ret == -1) {
		fprintf(stderr, "bind failed: %s\n", strerror(errno));
		exit(1);
	}

	ret = listen(socket_fd, 5);

	if(ret == -1) {
		fprintf(stderr, "listen failed: %s\n", strerror(errno));
		exit(1);
	}

	return socket_fd;
}

int main() {
	memset(&connect_addr, 0, sizeof(connect_addr));
	connect_addr.sin_family = AF_INET;
	connect_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	connect_addr.sin_port = htons(TEST_PORT);

	for(size_t i = 0; i < 4; i++) {
		listen_fd = socket_setup();
		if(!run_test(permutations[i])) {
			exit(1);
		}
		close(listen_fd);
	}

	return 0;
}

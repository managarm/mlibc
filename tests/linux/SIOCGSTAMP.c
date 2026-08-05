#include <assert.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int main(void) {
	int rx_fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(rx_fd >= 0);

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = 0;

	int rc = bind(rx_fd, (struct sockaddr *)&addr, sizeof(addr));
	assert(rc == 0);

	socklen_t len = sizeof(addr);
	rc = getsockname(rx_fd, (struct sockaddr *)&addr, &len);
	assert(rc == 0);

	int tx_fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(tx_fd >= 0);

	const char msg[] = "ping";
	ssize_t sent = sendto(tx_fd, msg, sizeof(msg), 0, (struct sockaddr *)&addr, sizeof(addr));
	assert(sent == (ssize_t)sizeof(msg));

	char buf[16];
	ssize_t received = recv(rx_fd, buf, sizeof(buf), 0);
	assert(received == (ssize_t)sizeof(msg));

	struct timeval tv;
	rc = ioctl(rx_fd, SIOCGSTAMP, &tv);
	assert(rc == 0);
	assert(tv.tv_sec > 0);

	close(rx_fd);
	close(tx_fd);

	return 0;
}

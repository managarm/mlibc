#include <netdb.h>
#include <assert.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <string.h>

int main() {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	assert(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr));

	char host[64];
	assert(!getnameinfo((struct sockaddr*)&addr, sizeof(addr), host,
				sizeof(host), NULL, 0, 0));

	assert(inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr));

	assert(!getnameinfo((struct sockaddr*)&addr, sizeof(addr), host,
				sizeof(host), NULL, 0, 0));
	assert(!strcmp(host, "dns.google"));
	return 0;
}

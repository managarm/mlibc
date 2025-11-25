#include <netdb.h>
#include <assert.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

int main() {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	assert(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr));

	char host[INET_ADDRSTRLEN + 1];
	assert(!getnameinfo((struct sockaddr*)&addr, sizeof(addr), host,
				sizeof(host), NULL, 0, 0));

	assert(inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr));

	assert(!getnameinfo((struct sockaddr*)&addr, sizeof(addr), host,
				sizeof(host), NULL, 0, 0));
	assert(!strcmp(host, "dns.google"));

	char service[64];
	addr.sin_port = htons(53);
	assert(!getnameinfo((struct sockaddr*)&addr, sizeof(addr), host,
			sizeof(host), service, sizeof(service), 0));
	assert(!strcmp(host, "dns.google"));
	assert(!strcmp(service, "domain"));

	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(1337);
	assert(!getnameinfo(
	    (struct sockaddr *)&addr,
	    sizeof(addr),
	    host,
	    sizeof(host),
	    service,
	    sizeof(service),
	    NI_NUMERICHOST | NI_NUMERICSERV
	));
	assert(!strcmp("127.0.0.1", host));
	assert(!strcmp("1337", service));

	addr.sin_addr.s_addr = 0;
	assert(!getnameinfo(
	    (struct sockaddr *)&addr,
	    sizeof(addr),
	    host,
	    sizeof(host),
	    service,
	    sizeof(service),
	    NI_NUMERICHOST | NI_NUMERICSERV
	));
	assert(!strcmp("0.0.0.0", host));
	assert(!strcmp("1337", service));

	return 0;
}

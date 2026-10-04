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

	// An address that resolves to no name is reported in its numeric form
	// rather than as an error. Only NI_NAMEREQD makes that a failure.
	// 192.0.2.0/24 is reserved for documentation and has no reverse record.
	struct sockaddr_in unresolvable;
	memset(&unresolvable, 0, sizeof(unresolvable));
	unresolvable.sin_family = AF_INET;
	assert(inet_pton(AF_INET, "192.0.2.1", &unresolvable.sin_addr));

	assert(!getnameinfo((struct sockaddr *)&unresolvable, sizeof(unresolvable),
				host, sizeof(host), NULL, 0, 0));
	assert(!strcmp("192.0.2.1", host));

	assert(getnameinfo((struct sockaddr *)&unresolvable, sizeof(unresolvable),
				host, sizeof(host), NULL, 0, NI_NAMEREQD) == EAI_NONAME);

	// The same for an IPv4-mapped address
	struct sockaddr_in6 mapped;
	memset(&mapped, 0, sizeof(mapped));
	mapped.sin6_family = AF_INET6;
	mapped.sin6_port = htons(443);
	mapped.sin6_addr.s6_addr[10] = 0xFF;
	mapped.sin6_addr.s6_addr[11] = 0xFF;
	mapped.sin6_addr.s6_addr[12] = 127;
	mapped.sin6_addr.s6_addr[15] = 1;

	char host6[NI_MAXHOST];
	assert(!getnameinfo((struct sockaddr *)&mapped, sizeof(mapped), host6,
				sizeof(host6), NULL, 0, NI_NUMERICHOST));
	assert(!strcmp("::ffff:127.0.0.1", host6));

	// Whether this one resolves depends on the host, but it has to succeed.
	assert(!getnameinfo((struct sockaddr *)&mapped, sizeof(mapped), host6,
				sizeof(host6), NULL, 0, 0));

	return 0;
}

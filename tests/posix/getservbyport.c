#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int main() {
	struct servent *ret = getservbyport(htons(80), "tcp");
	assert(ret);
	assert(!strcmp("http", ret->s_name));
	assert(ret->s_port == htons(80));
	assert(!strcmp("tcp", ret->s_proto));

	ret = getservbyport(htons(80), NULL);
	assert(ret);
	assert(!strcmp("http", ret->s_name));
	assert(ret->s_port == htons(80));
	assert(!strcmp("tcp", ret->s_proto));

	ret = getservbyport(htons(6696), "udp");
	assert(ret);
	ret = getservbyport(htons(6696), "tcp");
	assert(!ret);

	ret = getservbyport(htons(0), NULL);
	assert(!ret);
	return 0;
}

#include <assert.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
	struct servent *ret = getservbyname("http", "tcp");
	assert(ret);
	assert(!strcmp("http", ret->s_name));
	assert(ret->s_port == htons(80));
	assert(!strcmp("tcp", ret->s_proto));

	ret = getservbyname("http", NULL);
	assert(ret);
	assert(!strcmp("http", ret->s_name));
	assert(ret->s_port == htons(80));
	assert(!strcmp("tcp", ret->s_proto));

	ret = getservbyname("babel", "udp");
	assert(ret);
	ret = getservbyname("babel", "tcp");
	assert(!ret);

	ret = getservbyname("", NULL);
	assert(!ret);
	return 0;
}

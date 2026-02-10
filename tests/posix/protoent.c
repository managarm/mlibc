#include <assert.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>

int main(void) {
	struct protoent *entry;
	bool tcp_found = 0;

	setprotoent(1337);

	while ((entry = getprotoent()) != NULL) {
		if (strcmp(entry->p_name, "tcp") == 0) {
			tcp_found = true;
			assert(entry->p_proto == 6);
		}
	}

	assert(tcp_found == 1);

	entry = getprotobyname("tcp");
	assert(entry);
	assert(!strcmp(entry->p_name, "tcp"));
	assert(entry->p_proto == 6);

	entry = getprotobynumber(6);
	assert(entry);
	assert(!strcmp(entry->p_name, "tcp"));
	assert(entry->p_proto == 6);

	endprotoent();

	return 0;
}

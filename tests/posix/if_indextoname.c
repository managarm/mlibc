#include <assert.h>
#include <net/if.h>
#include <stdio.h>

int main() {
	char name[IF_NAMESIZE];

	assert(name == if_indextoname(1, name));
	printf("test: name '%s'\n", name);
	assert(1 == if_nametoindex(name));

	return 0;
}

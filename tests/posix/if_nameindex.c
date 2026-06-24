#include <assert.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	struct if_nameindex *ni;

	ni = if_nameindex();
	assert(ni != NULL);

	for (struct if_nameindex *i = ni; !(i->if_index == 0 && i->if_name == NULL); i++) {
		fprintf(stderr, "[%d] '%s'\n", i->if_index, i->if_name);
		assert(i->if_index > 0);
		assert(i->if_name != NULL);
		assert(strlen(i->if_name) > 0);
	}

	if_freenameindex(ni);

	return 0;
}

#include <assert.h>
#include <link.h>
#include <string.h>

struct result {
	int found_ldso;
	int found_self;
};

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
	assert(size == sizeof(struct dl_phdr_info));
	struct result *found = (struct result *) data;

	if (!strncmp("ld.so", info->dlpi_name, 5))
		found->found_ldso = 1;
	if (!strncmp("", info->dlpi_name, 0))
		found->found_self = 1;
	return 0;
}

int main() {
	struct result found = { 0 };
	assert(!dl_iterate_phdr(callback, &found));
	assert(found.found_ldso == 1);
	assert(found.found_self == 1);
	return 0;
}

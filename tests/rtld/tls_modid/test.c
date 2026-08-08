#include <assert.h>
#include <link.h>
#include <stddef.h>
#include <string.h>

// Gives the executable a PT_TLS segment.
__thread int exe_tls = 42;

struct result {
	int found_self;
};

static const ElfW(Phdr) *find_tls_phdr(struct dl_phdr_info *info) {
	for (size_t i = 0; i < info->dlpi_phnum; i++) {
		if (info->dlpi_phdr[i].p_type == PT_TLS)
			return &info->dlpi_phdr[i];
	}
	return NULL;
}

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
	assert(size == sizeof(struct dl_phdr_info));
	struct result *found = (struct result *) data;

	// A module index of zero means that the object has no TLS segment.
	const ElfW(Phdr) *tls_phdr = find_tls_phdr(info);
	assert(!info->dlpi_tls_modid == !tls_phdr);

	if (strcmp("", info->dlpi_name))
		return 0;
	found->found_self++;

	// Linkers resolve the module index of the executable's own TLS symbols statically
	// to one, hence the runtime has to agree on that assignment.
	assert(info->dlpi_tls_modid == 1);

	// Check that the index really does select the executable's own TLS block.
	assert(tls_phdr);
	char *block = info->dlpi_tls_data;
	assert(block);
	assert((char *)&exe_tls >= block);
	assert((char *)&exe_tls + sizeof(exe_tls) <= block + tls_phdr->p_memsz);
	return 0;
}

int main() {
	assert(exe_tls == 42);

	struct result found = { 0 };
	assert(!dl_iterate_phdr(callback, &found));
	assert(found.found_self == 1);
	return 0;
}

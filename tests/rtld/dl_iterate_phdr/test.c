#include <assert.h>
#include <link.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>

#ifdef USE_HOST_LIBC
#define LDSO_PATTERN "ld-linux-"
#define LIBFOO "libnative-foo.so"
#define LIBBAR "libnative-bar.so"
#else
#define LDSO_PATTERN "ld.so"
#define LIBFOO "libfoo.so"
#define LIBBAR "libbar.so"
#endif

struct result {
	int found_ldso;
	int found_self;
	int found_foo;
	int found_bar;
};

static int ends_with(const char *suffix, const char *s) {
	size_t suffix_len = strlen(suffix);
	size_t s_len = strlen(s);
	if (s_len < suffix_len)
		return 0;
	else {
		return !strcmp(suffix, s + s_len - suffix_len);
	}
}

static int contains(const char *pattern, const char *s) {
	return !!strstr(s, pattern);
}

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
	assert(size == sizeof(struct dl_phdr_info));
	struct result *found = (struct result *) data;

	printf("%s\n", info->dlpi_name);
	fflush(stdout);

	if (ends_with("foo.so", info->dlpi_name))
		found->found_foo++;
	if (ends_with("bar.so", info->dlpi_name))
		found->found_bar++;
	if (contains(LDSO_PATTERN, info->dlpi_name))
		found->found_ldso++;

	if (!strcmp("", info->dlpi_name))
		found->found_self++;

	assert(info->dlpi_phdr);
	return 0;
}

int main() {
	struct result found = { 0 };
	assert(!dl_iterate_phdr(callback, &found));
	assert(found.found_ldso == 1);
	assert(found.found_self == 1);
	assert(found.found_foo == 0);
	assert(found.found_bar == 0);
	printf("---\n");

	memset(&found, 0, sizeof(found));
	void *bar = dlopen(LIBBAR, RTLD_LOCAL | RTLD_NOW);
	assert(bar);
	assert(!dl_iterate_phdr(callback, &found));
	assert(found.found_ldso == 1);
	assert(found.found_self == 1);
	assert(found.found_bar == 1);
	assert(found.found_foo == 1); // Since bar depends on foo.
	printf("---\n");

	memset(&found, 0, sizeof(found));
	void *foo = dlopen(LIBFOO, RTLD_GLOBAL | RTLD_NOW);
	assert(foo);
	assert(!dl_iterate_phdr(callback, &found));
	assert(found.found_ldso == 1);
	assert(found.found_self == 1);
	assert(found.found_foo == 1);
	assert(found.found_bar == 1);
	printf("---\n");

	dlclose(bar);
	dlclose(foo);
	return 0;
}

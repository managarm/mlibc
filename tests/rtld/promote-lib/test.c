#include <dlfcn.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_HOST_LIBC
#define LIBONE "libnative-one.so"
#define LIBTWO "libnative-two.so"
#else
#define LIBONE "libone.so"
#define LIBTWO "libtwo.so"
#endif

static char *library_path(const char *name) {
	const char *dir = getenv("MLIBC_RTLD_PROMOTE_LIB_DIR");
	assert(dir);

	size_t size = strlen(dir) + 1 + strlen(name) + 1;
	char *path = malloc(size);
	assert(path);

	snprintf(path, size, "%s/%s", dir, name);
	return path;
}

int main() {
    char *libtwo_path = library_path(LIBTWO);

    printf("dlopen'ing libtwo by path with RTLD_LOCAL...\n");
    void *libtwo_local = dlopen(libtwo_path, RTLD_LOCAL | RTLD_NOW);
    assert(libtwo_local);
    assert(dlopen(libtwo_path, RTLD_NOLOAD | RTLD_NOW) == libtwo_local);

    printf("dlopen'ing libtwo by path with RTLD_GLOBAL...\n");
    void *libtwo_global = dlopen(libtwo_path, RTLD_GLOBAL | RTLD_NOW);
    assert(libtwo_global);
    assert(libtwo_local == libtwo_global);
    assert(dlopen(libtwo_path, RTLD_NOLOAD | RTLD_GLOBAL | RTLD_NOW) == libtwo_local);

    assert(dlsym(RTLD_DEFAULT, "libtwo_symbol") != NULL);

    printf("dlopen'ing libone...\n");
    void *libone = dlopen(LIBONE, RTLD_NOW);
    if (!libone) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
    }
    assert(libone);

    int (*libone_entry)(void);
    libone_entry = dlsym(libone, "libone_entry");
    assert(libone_entry);

    printf("Calling libone_entry...\n");
    int result = libone_entry();
    printf("libone_entry returned %d\n", result);
    assert(result == 42);

    free(libtwo_path);
    dlclose(libone);
    dlclose(libtwo_global);

    return 0;
}

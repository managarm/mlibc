#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>

#ifdef USE_HOST_LIBC
#define LIBONE "libnative-one.so"
#define LIBTWO "libnative-two.so"
#else
#define LIBONE "libone.so"
#define LIBTWO "libtwo.so"
#endif

int main() {
    printf("dlopen'ing libtwo with RTLD_LOCAL...\n");
    void *libtwo_local = dlopen(LIBTWO, RTLD_LOCAL | RTLD_NOW);
    assert(libtwo_local);

    printf("dlopen'ing libtwo with RTLD_GLOBAL...\n");
    void *libtwo_global = dlopen(LIBTWO, RTLD_GLOBAL | RTLD_NOW);
    assert(libtwo_global);
    assert(libtwo_local == libtwo_global);

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

    dlclose(libone);
    dlclose(libtwo_global);

    return 0;
}

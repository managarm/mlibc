#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>
#include <mlibc/allocator.hpp>
#include <mlibc/sigma-posix.hpp>
#include <frg/eternal.hpp>
#include <libsigma/sys.h>
#include <sys/auxv.h>

void __mlibc_initLocale();

extern "C" uintptr_t* __dlapi_entrystack();

extern char** environ;
static mlibc::exec_stack_data __mlibc_stack_data;

handle_t getUmRing(){
    return getauxval(AT_VFS_SERVER);
}

MemoryAllocator& getSysdepsAllocator(){
    static frg::eternal<VirtualAllocator> virtualAllocator;
    static frg::eternal<MemoryPool> heap{virtualAllocator.get()};
    static frg::eternal<MemoryAllocator> singleton{&heap.get()};
    return singleton.get();
}

struct LibraryGuard {
    LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard(){
    __mlibc_initLocale();

    mlibc::parse_exec_stack(__dlapi_entrystack(), &__mlibc_stack_data);
    mlibc::set_startup_data(__mlibc_stack_data.argc, __mlibc_stack_data.argv, __mlibc_stack_data.envp);
}

extern "C" void __mlibc_entry(int (*main_fn)(int argc, char *argv[], char *env[])){
    exit(main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ));
}

#include <cryptix/syscall.h>
#include <mlibc/elf/startup.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// defined by the POSIX library
void                          __mlibc_initLocale();

extern "C"
{
    unsigned int __mlibc_dev_major(unsigned long long int __dev)
    {
        return ((__dev >> 8) & 0xfff) | ((unsigned int)(__dev >> 32) & ~0xfff);
    }

    unsigned int __mlibc_dev_minor(unsigned long long int __dev)
    {
        return (__dev & 0xff) | ((unsigned int)(__dev >> 12) & ~0xff);
    }

    unsigned long long int __mlibc_dev_makedev(unsigned int __major,
                                               unsigned int __minor)
    {
        return ((__minor & 0xff) | ((__major & 0xfff) << 8)
                | (((unsigned long long int)(__minor & ~0xff)) << 12)
                | (((unsigned long long int)(__major & ~0xfff)) << 32));
    }
}

extern "C" uintptr_t*         __dlapi_entrystack();
extern "C" void               __dlapi_enter(uintptr_t*);

extern char**                 environ;
static mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard
{
    LibraryGuard();
};

static LibraryGuard guard;
namespace mlibc
{

    int sys_anon_allocate(size_t size, void** pointer);
}

LibraryGuard::LibraryGuard()
{
    __mlibc_initLocale();

    // Parse the exec() stack.
    mlibc::parse_exec_stack(__dlapi_entrystack(), &__mlibc_stack_data);
    mlibc::set_startup_data(__mlibc_stack_data.argc, __mlibc_stack_data.argv,
                            __mlibc_stack_data.envp);
}

extern "C" void __mlibc_entry(uintptr_t* entry_stack,
                              int        (*main_fn)(int argc, char* argv[],
                                             char* env[]))
{
    __dlapi_enter(entry_stack);

    auto result
        = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
    exit(result);
}

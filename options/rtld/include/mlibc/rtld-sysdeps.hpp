#ifndef MLIBC_RTLD_SYSDEPS
#define MLIBC_RTLD_SYSDEPS

namespace [[gnu::visibility("hidden")]] mlibc {

int sys_tcb_set(void *pointer);

[[gnu::weak]] int sys_vm_readahead(void *pointer, size_t size);

} // namespace mlibc

#endif // MLIBC_RTLD_SYSDEPS

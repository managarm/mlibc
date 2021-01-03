#ifndef MLIBC_RTDL_SYSDEPS
#define MLIBC_RTDL_SYSDEPS

namespace [[gnu::visibility("hidden")]] mlibc {

int sys_tcb_set(void *pointer);

[[gnu::weak]] int sys_vm_readahead(void *pointer, size_t size);

} // namespace mlibc

#endif // MLIBC_RTDL_SYSDEPS

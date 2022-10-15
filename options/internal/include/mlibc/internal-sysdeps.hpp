#ifndef MLIBC_INTERNAL_SYSDEPS
#define MLIBC_INTERNAL_SYSDEPS

#include <stddef.h>

#include <abi-bits/seek-whence.h>
#include <abi-bits/vm-flags.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <abi-bits/stat.h>
#include <mlibc/fsfd_target.hpp>

namespace [[gnu::visibility("hidden")]] mlibc {

void sys_libc_log(const char *message);
[[noreturn]] void sys_libc_panic();

int sys_tcb_set(void *pointer);

[[gnu::weak]] int sys_futex_tid();
int sys_futex_wait(int *pointer, int expected, const struct timespec *time);
int sys_futex_wake(int *pointer);

int sys_anon_allocate(size_t size, void **pointer);
int sys_anon_free(void *pointer, size_t size);

int sys_open(const char *pathname, int flags, mode_t mode, int *fd);
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
int sys_close(int fd);

[[gnu::weak]] int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
		struct stat *statbuf);
// mlibc assumes that anonymous memory returned by sys_vm_map() is zeroed by the kernel / whatever is behind the sysdeps
int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);
int sys_vm_unmap(void *pointer, size_t size);
[[gnu::weak]] int sys_vm_protect(void *pointer, size_t size, int prot);

} //namespace mlibc

#endif // MLIBC_INTERNAL_SYSDEPS

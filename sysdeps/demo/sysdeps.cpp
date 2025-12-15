#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#define SYS_EXIT 0
#define SYS_WRITE 1
#define SYS_MMAP 2

// ANCHOR: stub
#define STUB()                                                                 \
  ({                                                                           \
    __ensure(!"STUB function was called");                                     \
    __builtin_unreachable();                                                   \
  })
// ANCHOR_END: stub

namespace mlibc {

void sys_libc_panic() {
  sys_libc_log("!!! mlibc panic !!!");
  sys_exit(-1);
  __builtin_trap();
}

void sys_libc_log(const char *msg) {
  ssize_t unused;
  sys_write(2, msg, strlen(msg), &unused);
}

int sys_isatty(int fd) {
  (void)fd;
  // this returns ENOTTY when it is not a tty, but we do not have a proper implementation
  // so always return that a file is a tty
  return 0;
}

int sys_write(int fd, void const *buf, size_t size, ssize_t *ret) {

  *ret = syscall(SYS_WRITE, fd, buf, size);
  // this can never fail in the demo os
  return 0;
}

int sys_tcb_set(void *pointer) {
  uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
  asm volatile("mv tp, %0" ::"r"(thread_data));
  // this can never fail in the demo os
  return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
  auto out = syscall(SYS_MMAP, nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  *pointer = (void *)out;
  if (*pointer == MAP_FAILED)
    return ENOMEM; // the syscall does not return a proper errno, so use ENOMEM
  return 0;
}

int sys_anon_free(void *, unsigned long) { return 0; } // no-op

int sys_seek(int , off_t , int , off_t *) {
  return ESPIPE; // no proper file implementation, everything is a tty so return ESPIPE
}

void sys_exit(int status) {
  syscall(SYS_EXIT, status);
  __builtin_unreachable();
}

int sys_close(int ) { STUB(); }
int sys_futex_wake(int *) { STUB(); }
int sys_futex_wait(int *, int, timespec const *) { STUB(); }
int sys_read(int, void *, unsigned long, long *) { STUB(); }
int sys_open(const char *, int, unsigned int, int *) { STUB(); }
int sys_vm_map(void *, size_t , int , int , int ,
               off_t , void **) {
  STUB();
}
int sys_vm_unmap(void *, size_t) { STUB(); }
int sys_clock_get(int , time_t *, long *) { STUB(); }

} // namespace mlibc

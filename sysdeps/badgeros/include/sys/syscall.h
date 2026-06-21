
// WARNING: This is a generated file, do not edit it!
// SPDX-License-Identifier: CC0

#pragma once

#include <abi-bits/pid_t.h>
#include <abi-bits/signal.h>
#include <abi-bits/sigset_t.h>
#include <abi-bits/stat.h>
#include <abi-bits/tid_t.h>
#include <bits/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int __syscall_thread_yield_time(
);

int __syscall_thread_sleep(
    __mlibc_uint64 __delay
);

tid_t __syscall_thread_create(
    void * __entry,
    void * __arg,
    __mlibc_uint32 __priority
);

int __syscall_thread_detach(
    tid_t __thread_id
);

int __syscall_thread_join(
    tid_t __thread_id
);

int __syscall_thread_exit(
    int __code
);

int __syscall_proc_exit(
    int __code
);

pid_t __syscall_proc_fork(
);

int __syscall_proc_exec(
    char const * __path,
    char const *const * __argv,
    char const *const * __envp
);

int __syscall_proc_sigaction(
    int __signum,
    struct sigaction const *__newhandler,
    struct sigaction *__oldhandler
);

int __syscall_proc_sigret(
);

pid_t __syscall_proc_waitpid(
    pid_t __pid,
    int *__wstatus,
    int __options
);

int __syscall_fs_open(
    int __at,
    char const * __path,
    int __oflags
);

int __syscall_fs_close(
    int __fd
);

__mlibc_ptrdiff __syscall_fs_read(
    int __fd,
    __mlibc_uint8 *__read_buf, __mlibc_size __read_buf_len
);

__mlibc_ptrdiff __syscall_fs_write(
    int __fd,
    __mlibc_uint8 const *__write_buf, __mlibc_size __write_buf_len
);

__mlibc_ptrdiff __syscall_fs_getdents(
    int __fd,
    __mlibc_uint8 *__read_buf, __mlibc_size __read_buf_len
);

int __syscall_fs_rename(
    int __old_at,
    char const * __old_path,
    int __new_at,
    char const * __new_path,
    __mlibc_uint32 __flags
);

int __syscall_fs_stat(
    int __fd,
    char const * __path,
    bool __follow_link,
    struct stat *__stat_out
);

int __syscall_fs_mkdir(
    int __at,
    char const * __path
);

int __syscall_fs_rmdir(
    int __at,
    char const * __path
);

int __syscall_fs_link(
    int __old_at,
    char const * __old_path,
    int __new_at,
    char const * __new_path,
    __mlibc_uint32 __flags
);

int __syscall_fs_unlink(
    int __at,
    char const * __path
);

int __syscall_fs_mkfifo(
    int __at,
    char const * __path
);

int __syscall_fs_pipe(
    int __fds[2],
    int __flags
);

__mlibc_int64 __syscall_fs_seek(
    int __fd,
    __mlibc_int64 __offset,
    int __whence
);

__mlibc_ptrdiff __syscall_mem_map(
    void * __address,
    __mlibc_size __size,
    int __prot,
    int __flags,
    int __fd,
    __mlibc_int64 __offset
);

int __syscall_mem_unmap(
    void * __address,
    __mlibc_size __size
);

int __syscall_mem_protect(
    void * __address,
    __mlibc_size __size,
    int __prot
);

int __syscall_sys_log(
    char const *__message, __mlibc_size __message_len
);

int __syscall_time_gettime(
    int __clkid,
    struct timespec *__time
);

int __syscall_thread_kill(
    tid_t __thread_id,
    int __signum
);

int __syscall_proc_kill(
    pid_t __pid,
    int __signum
);

__mlibc_int64 __syscall_proc_getid(
    int __getid_type
);

int __syscall_fs_symlink(
    char const * __link_target,
    int __at,
    char const * __path
);

int __syscall_fs_dup(
    int __fd,
    int __flags,
    int __newfd
);

int __syscall_thread_sigmask(
    int __how,
    sigset_t const *__set,
    sigset_t *__oldset
);

int __syscall_sys_uname(
    struct utsname *__name
);

int __syscall_fs_isatty(
    int __fd
);

int __syscall_fs_tcgetattr(
    int __fd,
    struct termios *__attr
);

int __syscall_fs_tcsetattr(
    int __fd,
    struct termios const *__attr
);

int __syscall_fs_getcwd(
    char *__buf, __mlibc_size __buf_len
);

int __syscall_fs_chdir(
    int __at,
    char const * __path
);

int __syscall_fs_getfd(
    int __fd
);

int __syscall_fs_setfd(
    int __fd,
    int __flags
);

int __syscall_fs_getfl(
    int __fd
);

int __syscall_fs_setfl(
    int __fd,
    int __flags
);

#ifdef __cplusplus
} // extern "C"
#endif

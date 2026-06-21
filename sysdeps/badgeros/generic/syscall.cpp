
// WARNING: This is a generated file, do not edit it!
// SPDX-License-Identifier: CC0

#include <sys/syscall.h>
#include <sys/do_syscall_asm.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

__attribute__((naked))
int __syscall_thread_yield_time(
) {
    _DO_SYSCALL_ASM(0);
}

__attribute__((naked))
int __syscall_thread_sleep(
    __mlibc_uint64 __delay
) {
    _DO_SYSCALL_ASM(1);
}

__attribute__((naked))
tid_t __syscall_thread_create(
    void * __entry,
    void * __arg,
    __mlibc_uint32 __priority
) {
    _DO_SYSCALL_ASM(2);
}

__attribute__((naked))
int __syscall_thread_detach(
    tid_t __thread_id
) {
    _DO_SYSCALL_ASM(3);
}

__attribute__((naked))
int __syscall_thread_join(
    tid_t __thread_id
) {
    _DO_SYSCALL_ASM(4);
}

__attribute__((naked))
int __syscall_thread_exit(
    int __code
) {
    _DO_SYSCALL_ASM(5);
}

__attribute__((naked))
int __syscall_proc_exit(
    int __code
) {
    _DO_SYSCALL_ASM(6);
}

__attribute__((naked))
pid_t __syscall_proc_fork(
) {
    _DO_SYSCALL_ASM(7);
}

__attribute__((naked))
int __syscall_proc_exec(
    char const * __path,
    char const *const * __argv,
    char const *const * __envp
) {
    _DO_SYSCALL_ASM(8);
}

__attribute__((naked))
int __syscall_proc_sigaction(
    int __signum,
    struct sigaction const *__newhandler,
    struct sigaction *__oldhandler
) {
    _DO_SYSCALL_ASM(9);
}

__attribute__((naked))
int __syscall_proc_sigret(
) {
    _DO_SYSCALL_ASM(10);
}

__attribute__((naked))
pid_t __syscall_proc_waitpid(
    pid_t __pid,
    int *__wstatus,
    int __options
) {
    _DO_SYSCALL_ASM(11);
}

__attribute__((naked))
int __syscall_fs_open(
    int __at,
    char const * __path,
    int __oflags
) {
    _DO_SYSCALL_ASM(12);
}

__attribute__((naked))
int __syscall_fs_close(
    int __fd
) {
    _DO_SYSCALL_ASM(13);
}

__attribute__((naked))
__mlibc_ptrdiff __syscall_fs_read(
    int __fd,
    __mlibc_uint8 *__read_buf, __mlibc_size __read_buf_len
) {
    _DO_SYSCALL_ASM(14);
}

__attribute__((naked))
__mlibc_ptrdiff __syscall_fs_write(
    int __fd,
    __mlibc_uint8 const *__write_buf, __mlibc_size __write_buf_len
) {
    _DO_SYSCALL_ASM(15);
}

__attribute__((naked))
__mlibc_ptrdiff __syscall_fs_getdents(
    int __fd,
    __mlibc_uint8 *__read_buf, __mlibc_size __read_buf_len
) {
    _DO_SYSCALL_ASM(16);
}

__attribute__((naked))
int __syscall_fs_rename(
    int __old_at,
    char const * __old_path,
    int __new_at,
    char const * __new_path,
    __mlibc_uint32 __flags
) {
    _DO_SYSCALL_ASM(17);
}

__attribute__((naked))
int __syscall_fs_stat(
    int __fd,
    char const * __path,
    bool __follow_link,
    struct stat *__stat_out
) {
    _DO_SYSCALL_ASM(18);
}

__attribute__((naked))
int __syscall_fs_mkdir(
    int __at,
    char const * __path
) {
    _DO_SYSCALL_ASM(19);
}

__attribute__((naked))
int __syscall_fs_rmdir(
    int __at,
    char const * __path
) {
    _DO_SYSCALL_ASM(20);
}

__attribute__((naked))
int __syscall_fs_link(
    int __old_at,
    char const * __old_path,
    int __new_at,
    char const * __new_path,
    __mlibc_uint32 __flags
) {
    _DO_SYSCALL_ASM(21);
}

__attribute__((naked))
int __syscall_fs_unlink(
    int __at,
    char const * __path
) {
    _DO_SYSCALL_ASM(22);
}

__attribute__((naked))
int __syscall_fs_mkfifo(
    int __at,
    char const * __path
) {
    _DO_SYSCALL_ASM(23);
}

__attribute__((naked))
int __syscall_fs_pipe(
    int __fds[2],
    int __flags
) {
    _DO_SYSCALL_ASM(24);
}

__attribute__((naked))
__mlibc_int64 __syscall_fs_seek(
    int __fd,
    __mlibc_int64 __offset,
    int __whence
) {
    _DO_SYSCALL_ASM(25);
}

__attribute__((naked))
__mlibc_ptrdiff __syscall_mem_map(
    void * __address,
    __mlibc_size __size,
    int __prot,
    int __flags,
    int __fd,
    __mlibc_int64 __offset
) {
    _DO_SYSCALL_ASM(26);
}

__attribute__((naked))
int __syscall_mem_unmap(
    void * __address,
    __mlibc_size __size
) {
    _DO_SYSCALL_ASM(27);
}

__attribute__((naked))
int __syscall_mem_protect(
    void * __address,
    __mlibc_size __size,
    int __prot
) {
    _DO_SYSCALL_ASM(28);
}

__attribute__((naked))
int __syscall_sys_log(
    char const *__message, __mlibc_size __message_len
) {
    _DO_SYSCALL_ASM(29);
}

__attribute__((naked))
int __syscall_time_gettime(
    int __clkid,
    struct timespec *__time
) {
    _DO_SYSCALL_ASM(30);
}

__attribute__((naked))
int __syscall_thread_kill(
    tid_t __thread_id,
    int __signum
) {
    _DO_SYSCALL_ASM(31);
}

__attribute__((naked))
int __syscall_proc_kill(
    pid_t __pid,
    int __signum
) {
    _DO_SYSCALL_ASM(32);
}

__attribute__((naked))
__mlibc_int64 __syscall_proc_getid(
    int __getid_type
) {
    _DO_SYSCALL_ASM(33);
}

__attribute__((naked))
int __syscall_fs_symlink(
    char const * __link_target,
    int __at,
    char const * __path
) {
    _DO_SYSCALL_ASM(34);
}

__attribute__((naked))
int __syscall_fs_dup(
    int __fd,
    int __flags,
    int __newfd
) {
    _DO_SYSCALL_ASM(35);
}

__attribute__((naked))
int __syscall_thread_sigmask(
    int __how,
    sigset_t const *__set,
    sigset_t *__oldset
) {
    _DO_SYSCALL_ASM(36);
}

__attribute__((naked))
int __syscall_sys_uname(
    struct utsname *__name
) {
    _DO_SYSCALL_ASM(37);
}

__attribute__((naked))
int __syscall_fs_isatty(
    int __fd
) {
    _DO_SYSCALL_ASM(38);
}

__attribute__((naked))
int __syscall_fs_tcgetattr(
    int __fd,
    struct termios *__attr
) {
    _DO_SYSCALL_ASM(39);
}

__attribute__((naked))
int __syscall_fs_tcsetattr(
    int __fd,
    struct termios const *__attr
) {
    _DO_SYSCALL_ASM(40);
}

__attribute__((naked))
int __syscall_fs_getcwd(
    char *__buf, __mlibc_size __buf_len
) {
    _DO_SYSCALL_ASM(41);
}

__attribute__((naked))
int __syscall_fs_chdir(
    int __at,
    char const * __path
) {
    _DO_SYSCALL_ASM(42);
}

__attribute__((naked))
int __syscall_fs_getfd(
    int __fd
) {
    _DO_SYSCALL_ASM(43);
}

__attribute__((naked))
int __syscall_fs_setfd(
    int __fd,
    int __flags
) {
    _DO_SYSCALL_ASM(44);
}

__attribute__((naked))
int __syscall_fs_getfl(
    int __fd
) {
    _DO_SYSCALL_ASM(45);
}

__attribute__((naked))
int __syscall_fs_setfl(
    int __fd,
    int __flags
) {
    _DO_SYSCALL_ASM(46);
}

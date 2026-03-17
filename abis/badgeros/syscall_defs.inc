
// SPDX-License-Identifier: MIT
// clang-format off



/* ==== DEFAULT MACRO EXPANSIONS ==== */

// Normal syscall definition.
#ifndef _SYSCALL_DEF
#define _SYSCALL_DEF(__no, __enum, __name, __returns, ...)
#endif

// Normal syscall definition (returning void).
#ifndef _SYSCALL_DEF_V
#define _SYSCALL_DEF_V(__no, __enum, __name, ...) _SYSCALL_DEF(__no, __enum, __name, void, __VA_ARGS__)
#endif




// Yield to other threads.
_SYSCALL_DEF_V(0, _SYSCALL_THREAD_YIELD, __syscall_thread_yield)

// Sleep for a certain amount of time.
_SYSCALL_DEF_V(1, _SYSCALL_THREAD_SLEEP, __syscall_thread_sleep, __mlibc_uint64 __delay)

// Create a new thread.
// Returns thread ID or -errno.
_SYSCALL_DEF(2, _SYSCALL_THREAD_CREATE, __syscall_thread_create, long, void *__entry, void *__arg, int __priority)

// Detach a thread; the thread will be destroyed as soon as it exits.
// Returns 0 or -errno.
_SYSCALL_DEF(3, _SYSCALL_THREAD_DETACH, __syscall_thread_detach, int, long __thread_id)

// Wait for a thread to stop and return its exit code.
// Returns the exit code of that thread or -errno.
_SYSCALL_DEF(4, _SYSCALL_THREAD_JOIN, __syscall_thread_join, int, long __thread_id)

// Exit the current thread; exit code can be read unless destroyed or detached.
_SYSCALL_DEF_V(5, _SYSCALL_THREAD_EXIT, __syscall_thread_exit, int __code)

// Exit the process; exit code can be read by parent process.
_SYSCALL_DEF_V(6, _SYSCALL_PROC_EXIT, __syscall_proc_exit, int __code)

// Get the command-line arguments (i.e. argc+argv) of the current process.
// If memory is large enough, a NULL-terminated argv array of C-string pointers and their data is stored in `memory`.
// The function returns how many bytes would be needed to store the structure.
// If the memory was not large enough, it it not modified.
_SYSCALL_DEF(7, _SYSCALL_PROC_GETARGS, __syscall_proc_getargs, __mlibc_size, __mlibc_size __cap, void *__memory)

// Create a copy of the running process and return its PID (to the parent) or -1 (to the child).
_SYSCALL_DEF(8, _SYSCALL_PROC_FORK, __syscall_proc_fork, __mlibc_int64)

// Execute the program at `path`, replacing the calling program's code and data in the process.
_SYSCALL_DEF(9, _SYSCALL_PROC_EXEC, __syscall_proc_exec, int, char const *__path, char const *const *__argv, char const *const *__envp)

// Set the signal handler for a specific signal number.
_SYSCALL_DEF(10, _SYSCALL_PROC_SIGACTION, __syscall_proc_sigaction, int, int __signum, struct sigaction const *__newhandler, struct sigaction *__oldhandler)

// Return from a signal handler.
_SYSCALL_DEF_V(11, _SYSCALL_PROC_SIGRET, __syscall_proc_sigret)

// Get child process status update.
_SYSCALL_DEF(12, _SYSCALL_PROC_WAITPID, __syscall_proc_waitpid, __mlibc_int64, __mlibc_int64 __pid, int *__wstatus, int __options)

// Open a file, optionally relative to a directory.
// Returns -errno on error, file descriptor number on success.
_SYSCALL_DEF(13, _SYSCALL_FS_OPEN, __syscall_fs_open, int, int __at, char const *__path, int __oflags)

// Flush and close a file.
_SYSCALL_DEF(14, _SYSCALL_FS_CLOSE, __syscall_fs_close, int, int __fd)

// Read bytes from a file.
// Returns 0 on EOF, -errno on error, read count on success.
_SYSCALL_DEF(15, _SYSCALL_FS_READ, __syscall_fs_read, __mlibc_int64, int __fd, void *__read_buf, long read_len)

// Write bytes to a file.
// Returns -errno on error, write count on success.
_SYSCALL_DEF(16, _SYSCALL_FS_WRITE, __syscall_fs_write, __mlibc_int64, int __fd, void const *__write_buf, long write_len)

// Read directory entries from a directory handle.
// See `dirent_t` for the format.
// Returns -errno on error, read count on success.
_SYSCALL_DEF(17, _SYSCALL_FS_GETDENTS, __syscall_fs_getdents, __mlibc_ptrdiff, int __fd, void *__read_buf, __mlibc_size __read_len)

// Rename and/or move a file to another path, optionally relative to one or two directories.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(18, _SYSCALL_FS_RENAME, __syscall_fs_rename, int, int __old_at, char const *__old_path, int __new_at, char const *__new_path, __mlibc_uint32 __flags)
    
// Get file status given file handler or path, optionally following the final symlink.
// If `path` is specified, it is interpreted as relative to the working directory.
// If both `path` and `fd` are specified, `path` is relative to the directory that `fd` describes.
// If only `fd` is specified, the inode referenced by `fd` is stat'ed.
// If `follow_link` is false, the last symlink in the path is not followed.
_SYSCALL_DEF(19, _SYSCALL_FS_STAT, __syscall_fs_stat, int, int __fd, char const *__path, bool __follow_link, struct stat *__stat_out)

// Create a new directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(20, _SYSCALL_FS_MKDIR, __syscall_fs_mkdir, int, int __at, char const *__path)

// Delete a directory if it is empty.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(21, _SYSCALL_FS_RMDIR, __syscall_fs_rmdir, int, int __at, char const *__path)

// Create a new link to an existing inode.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(22, _SYSCALL_FS_LINK, __syscall_fs_link, int, int __old_at, char const *__old_path, int __new_at, char const *__new_path, __mlibc_uint32 __flags)

// Remove a link to an inode. If it is the last link, the file is deleted.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(23, _SYSCALL_FS_UNLINK, __syscall_fs_unlink, int, int __at, char const *__path)

// Create a new FIFO / named pipe.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(24, _SYSCALL_FS_MKFIFO, __syscall_fs_mkfifo, int, int __at, char const *__path)

// Create a new pipe.
// `fds[0]` will be written with the pointer to the read end, `fds[1]` the write end.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(25, _SYSCALL_FS_PIPE, __syscall_fs_pipe, int, int __fds[2], int __flags)

_SYSCALL_DEF(26, _SYSCALL_FS_SEEK, __syscall_fs_seek, __mlibc_int64, int fd, __mlibc_int64 offset, int __whence)

// Map a new range of memory at an arbitrary virtual address.
// This may round up to a multiple of the page size.
// Alignment may be less than `align` if the kernel doesn't support it.
_SYSCALL_DEF(27, _SYSCALL_MEM_MAP, __syscall_mem_map, void *, void *__address, __mlibc_size __size, int __prot, int __flags)

// Unmap a range of memory previously allocated with `SYSCALL_MEM_MAP`.
// Returns whether a range of memory was unmapped.
_SYSCALL_DEF_V(28, _SYSCALL_MEM_UNMAP, __syscall_mem_unmap, void *__address, __mlibc_size __size)

// Change the protection flags on a range of memory.
// May fail if the process does not have write access to mapped objects.
_SYSCALL_DEF(29, _SYSCALL_MEM_PROTECT, __syscall_mem_protect, int, void *__address, __mlibc_size size, int __prot)

_SYSCALL_DEF_V(30, _SYSCALL_TEMP_WRITE, __syscall_temp_write, char const *__message, __mlibc_size __len)

// Start the shutdown process.
_SYSCALL_DEF_V(31, _SYSCALL_SYS_SHUTDOWN, __syscall_sys_shutdown, bool __is_reboot)

// Get the value of some clock.
_SYSCALL_DEF(32, _SYSCALL_TIME_GETTIME, __syscall_time_gettime, int, int __clkid, struct timespec *__time)

// Send a signal to a thread in this process.
_SYSCALL_DEF(33, _SYSCALL_THREAD_KILL, __syscall_thread_kill, int, long __thread_id, int __signum)

// Send a signal to an arbitrary thread in a specified process. See man kill(2).
_SYSCALL_DEF(34, _SYSCALL_PROC_KILL, __syscall_proc_kill, int, __mlibc_int64 __pid, int __signum)

// Get an ID as specified by _GETID_* macros.
_SYSCALL_DEF(35, _SYSCALL_PROC_GETID, __syscall_proc_getid, int, int __getid_type)

// Create a new directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(36, _SYSCALL_FS_SYMLINK, __syscall_fs_symlink, int, char const *__link_target, int __at, char const *__path)

// Duplicate a file descriptor.
// If `__newfd` is `-1`, an arbitrary descriptor number is selected.
_SYSCALL_DEF(37, _SYSCALL_FS_DUP, __syscall_fs_dup, int, int __fd, int __flags, int __newfd)



#undef _SYSCALL_DEF
#undef _SYSCALL_DEF_V

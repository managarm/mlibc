
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




/* ==== THREAD SYSCALLS ==== */
// Implemented in process/syscall_impl.c

// Yield to other threads.
_SYSCALL_DEF_V(1, _SYSCALL_THREAD_YIELD, __syscall_thread_yield)

// Sleep for a certain amount of time.
_SYSCALL_DEF_V(53, _SYSCALL_THREAD_SLEEP, __syscall_thread_sleep, __mlibc_uint64 __delay)

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



/* ==== PROCESS MANAGEMENT SYSCALLS ==== */
// Implemented in process/syscall_impl.c

// Exit the process; exit code can be read by parent process.
_SYSCALL_DEF_V(8, _SYSCALL_PROC_EXIT, __syscall_proc_exit, int __code)

// Get the command-line arguments (i.e. argc+argv) of the current process.
// If memory is large enough, a NULL-terminated argv array of C-string pointers and their data is stored in `memory`.
// The function returns how many bytes would be needed to store the structure.
// If the memory was not large enough, it it not modified.
_SYSCALL_DEF(9, _SYSCALL_PROC_GETARGS, __syscall_proc_getargs, __mlibc_size, __mlibc_size __cap, void *__memory)

// Create a copy of the running process and return its PID (to the parent) or -1 (to the child).
_SYSCALL_DEF(10, _SYSCALL_PROC_FORK, __syscall_proc_fork, __mlibc_int64)

// Execute the program at `path`, replacing the calling program's code and data in the process.
_SYSCALL_DEF(11, _SYSCALL_PROC_EXEC, __syscall_proc_exec, int, char const *__path, char const *const *__argv, char const *const *__envp)

// Reserved for POSIX spawn.
// _SYSCALL_DEF(12, _SYSCALL_PROC_SPAWN, int, ...)

// Set the signal handler for a specific signal number.
_SYSCALL_DEF(13, _SYSCALL_PROC_SIGACTION, __syscall_proc_sigaction, int, int __signum, struct sigaction const *__newhandler, struct sigaction *__oldhandler)

// Return from a signal handler.
_SYSCALL_DEF_V(14, _SYSCALL_PROC_SIGRET, __syscall_proc_sigret)

// Get child process status update.
_SYSCALL_DEF(15, _SYSCALL_PROC_WAITPID, __syscall_proc_waitpid, int, int __pid, int *__wstatus, int __options)


/* ==== FILESYSTEM SYSCALLS ==== */
// Implemented in filesystem/syscall_impl.c

// Open a file, optionally relative to a directory.
// If `at` is -1, `path` is relative to the working directory.
// Returns -errno on error, file descriptor number on success.
_SYSCALL_DEF(16, _SYSCALL_FS_OPEN, __syscall_fs_open, int, int __at, char const *__path, int oflags)

// Flush and close a file.
_SYSCALL_DEF(17, _SYSCALL_FS_CLOSE, __syscall_fs_close, int, int __fd)

// Read bytes from a file.
// Returns 0 on EOF, -errno on error, read count on success.
_SYSCALL_DEF(18, _SYSCALL_FS_READ, __syscall_fs_read, __mlibc_int64, int __fd, void *read_buf, long read_len)

// Write bytes to a file.
// Returns -errno on error, write count on success.
_SYSCALL_DEF(19, _SYSCALL_FS_WRITE, __syscall_fs_write, __mlibc_int64, int __fd, void const *write_buf, long write_len)

// Read directory entries from a directory handle.
// See `dirent_t` for the format.
// Returns -errno on error, read count on success.
_SYSCALL_DEF(20, _SYSCALL_FS_GETDENTS, __syscall_fs_getdents, __mlibc_int64, int __fd, void *read_buf, long read_len)

// Rename and/or move a file to another path, optionally relative to one or two directories.
// If `*_at` is -1, the respective `*_path` is relative to the working directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(21, _SYSCALL_FS_RENAME, __syscall_fs_rename, int, int __old_at, char const *__old_path, int __new_at, char const *__new_path, __mlibc_uint32 __flags)
    
// Get file status given file handler or path, optionally following the final symlink.
// If `path` is specified, it is interpreted as relative to the working directory.
// If both `path` and `fd` are specified, `path` is relative to the directory that `fd` describes.
// If only `fd` is specified, the inode referenced by `fd` is stat'ed.
// If `follow_link` is false, the last symlink in the path is not followed.
_SYSCALL_DEF(22, _SYSCALL_FS_STAT, __syscall_fs_stat, int, int __fd, char const *__path, bool follow_link, struct stat *stat_out)

// Create a new directory.
// If `at` is -1, `path` is relative to the working directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(47, _SYSCALL_FS_MKDIR, __syscall_fs_mkdir, int, int __at, char const *__path)

// Delete a directory if it is empty.
// If `at` is -1, `path` is relative to the working directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(48, _SYSCALL_FS_RMDIR, __syscall_fs_rmdir, int, int __at, char const *__path)

// Create a new link to an existing inode.
// If `*_at` is -1, the respective `*_path` is relative to the working directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(49, _SYSCALL_FS_LINK, __syscall_fs_link, int, int __old_at, char const *__old_path, int __new_at, char const *__new_path, __mlibc_uint32 __flags)

// Remove a link to an inode. If it is the last link, the file is deleted.
// If `at` is -1, `path` is relative to the working directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(50, _SYSCALL_FS_UNLINK, __syscall_fs_unlink, int, int __at, char const *__path)

// Create a new FIFO / named pipe.
// If `at` is -1, `path` is relative to the working directory.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(51, _SYSCALL_FS_MKFIFO, __syscall_fs_mkfifo, int, int __at, char const *__path)

// Create a new pipe.
// `fds[0]` will be written with the pointer to the read end, `fds[1]` the write end.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(52, _SYSCALL_FS_PIPE, __syscall_fs_pipe, int, int __fds[2], int __flags)

// Create a new pipe.
// `fds[0]` will be written with the pointer to the read end, `fds[1]` the write end.
// Returns -errno on error, 0 on success.
_SYSCALL_DEF(53, _SYSCALL_FS_SEEK, __syscall_fs_seek, __mlibc_int64, int fd, __mlibc_int64 offset, int whence)



/* ==== MEMORY MANAGEMENT SYSCALLS ==== */
// Implemented in process/syscall_impl.c

// Map a new range of memory at an arbitrary virtual address.
// This may round up to a multiple of the page size.
// Alignment may be less than `align` if the kernel doesn't support it.
_SYSCALL_DEF(23, _SYSCALL_MEM_MAP, __syscall_mem_map, void *, void *__address, __mlibc_size __size, int __prot, int __flags)

// Unmap a range of memory previously allocated with `SYSCALL_MEM_MAP`.
// Returns whether a range of memory was unmapped.
_SYSCALL_DEF_V(25, _SYSCALL_MEM_UNMAP, __syscall_mem_unmap, void *__address, __mlibc_size __size)

// Change the protection flags on a range of memory.
// May fail if the process does not have write access to mapped objects.
_SYSCALL_DEF(26, _SYSCALL_MEM_PROTECT, __syscall_mem_protect, int, void *__address, __mlibc_size size, int __prot)



_SYSCALL_DEF_V(46, _SYSCALL_TEMP_WRITE, __syscall_temp_write, char const *__message, __mlibc_size __len)



/* ==== SYSTEM MANAGEMENT SYSCALLS ==== */
// Implemented in main.c

// Start the shutdown process.
_SYSCALL_DEF_V(45, _SYSCALL_SYS_SHUTDOWN, __syscall_sys_shutdown, bool __is_reboot)



#undef _SYSCALL_DEF
#undef _SYSCALL_DEF_V

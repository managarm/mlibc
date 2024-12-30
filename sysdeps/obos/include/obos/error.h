#pragma once

#define obos_is_error(status) ((status) != OBOS_STATUS_SUCCESS)
#define obos_is_success(status) ((status) == OBOS_STATUS_SUCCESS)

typedef enum
{
	/// <summary>
	/// The function succeeded.
	/// </summary>
	OBOS_STATUS_SUCCESS,
	/// <summary>
	/// An invalid IRQL value was passed to the function.
	/// </summary>
	OBOS_STATUS_INVALID_IRQL,
	/// <summary>
	/// An invalid argument was passed to the function.
	/// </summary>
	OBOS_STATUS_INVALID_ARGUMENT,
	/// <summary>
	/// A function was called, but an option passed to it had not been implemented yet, or
	/// <para/>the function itself had not been implemented yet.
	/// </summary>
	OBOS_STATUS_UNIMPLEMENTED,
	/// <summary>
	/// A function was called before one of the prerequisite components was initialized.
	/// </summary>
	OBOS_STATUS_INVALID_INIT_PHASE,
	/// <summary>
	/// The affinity in the thread object is invalid.
	/// </summary>
	OBOS_STATUS_INVALID_AFFINITY,
	/// <summary>
	/// There is not enough memory to satisfy your request.
	/// </summary>
	OBOS_STATUS_NOT_ENOUGH_MEMORY,
	/// <summary>
	/// A mismatched pointer was passed.
	/// </summary>
	OBOS_STATUS_MISMATCH,
	/// <summary>
	/// An internal error happened.
	/// </summary>
	OBOS_STATUS_INTERNAL_ERROR,
	/// <summary>
	/// An error occurred, and a retry of the operation is needed.
	/// </summary>
	OBOS_STATUS_RETRY,
	/// <summary>
	/// The object is already initialized.
	/// </summary>
	OBOS_STATUS_ALREADY_INITIALIZED,
	/// <summary>
	/// The request could not be fulfilled, as a required resource was not found.
	/// </summary>
	OBOS_STATUS_NOT_FOUND,
	/// <summary>
	/// The resource is already in use.
	/// </summary>
	OBOS_STATUS_IN_USE,
	/// <summary>
	/// Access has been denied.
	/// </summary>
	OBOS_STATUS_ACCESS_DENIED,
	/// <summary>
	/// The object has not been initialized yet.
	/// </summary>
	OBOS_STATUS_UNINITIALIZED,
	/// <summary>
	/// The callback did not handle anything.
	/// </summary>
	OBOS_STATUS_UNHANDLED,
	/// <summary>
	/// The page cannot be paged in or out, as it's in the non-paged pool of memory.
	/// </summary>
	OBOS_STATUS_UNPAGED_POOL,
	/// <summary>
	/// The file has an invalid format.
	/// </summary>
	OBOS_STATUS_INVALID_FILE,
	/// <summary>
	/// The driver put an invalid value for a field in its header.
	/// </summary>
	OBOS_STATUS_INVALID_HEADER,
	/// <summary>
	/// The driver's code referenced an undefined symbol in the kernel.
	/// </summary>
	OBOS_STATUS_DRIVER_REFERENCED_UNRESOLVED_SYMBOL,
	/// <summary>
	/// The driver's code referenced a variable that had a different size in the kernel.
	/// </summary>
	OBOS_STATUS_DRIVER_SYMBOL_MISMATCH,
	/// <summary>
	/// The driver does not have an entry point.
	/// </summary>
	OBOS_STATUS_NO_ENTRY_POINT,
	/// <summary>
	/// The caller of driver->header.ftable.ioctl gave an invalid request.
	/// </summary>
	OBOS_STATUS_INVALID_IOCTL,
	/// <summary>
	/// The driver received an invalid operation.
	/// <para/>An example is if a pipe-style device called get_max_blk_count.
	/// </summary>
	OBOS_STATUS_INVALID_OPERATION,
	/// <summary>
	/// The DPC object was already enqueued before the call of the function.
	/// </summary>
	OBOS_STATUS_DPC_ALREADY_ENQUEUED,
	/// <summary>
	/// The lock was already locked by the current thread.
	/// </summary>
	OBOS_STATUS_RECURSIVE_LOCK,
	/// <summary>
	/// The driver cannot satisfy the write request, as the device/filesystem it's manipulating is read only.
	/// </summary>
	OBOS_STATUS_READ_ONLY,
	/// <summary>
	/// The filesystem driver cannot satisfy the request, as the descriptor passed was not a file.
	/// </summary>
	OBOS_STATUS_NOT_A_FILE,
	/// <summary>
	/// The vnode already had a node mounted there.
	/// </summary>
	OBOS_STATUS_ALREADY_MOUNTED,
	/// <summary>
	/// The fd object hit EOF.
	/// </summary>
	OBOS_STATUS_EOF,
	/// <summary>
	/// The operation was aborted.
	/// </summary>
	OBOS_STATUS_ABORTED,
	/// <summary>
	/// A page fault happened accessing user memory passed in a syscall or anywhere similar.
	/// </summary>
	OBOS_STATUS_PAGE_FAULT,
	/// <summary>
	/// A driver timed out.
	/// </summary>
	OBOS_STATUS_TIMED_OUT,
	/// <summary>
	/// The pipe has no more readers.<para/>
	/// Only returned if SIGPIPE is ignored by a process.
	/// </summary>
	OBOS_STATUS_PIPE_CLOSED,
	/// <summary>
	/// Returned if the write request could not be completed, as there is simply not enough space in the device<para/>
	/// For example, this can happen if the size of a write to a pipe is greater than the pipe's size.'
	/// </summary>
	OBOS_STATUS_NO_SPACE,
	/// <summary>
	/// There is no syscall at this syscall number :(
	/// </summary>
	OBOS_STATUS_NO_SYSCALL,
	/// <summary>
	/// The ACPI device is not capable to wake the system from the requested sleep state.
	/// </summary>
	OBOS_STATUS_WAKE_INCAPABLE,
	/// <summary>
	/// The ELF file has an invalid, or unsupported elf type in it's header.
	/// </summary>
	OBOS_STATUS_INVALID_ELF_TYPE,
} obos_status;

#include <mlibc/internal-sysdeps.hpp>

#include <obos/error.h>
#include <obos/syscall.h>

#include <errno.h>

namespace [[gnu::visibility("hidden")]] mlibc {

int sys_tcb_set(void *pointer) {
	syscall1(SysS_SetFSBase, pointer);
	return 0;
}

int sys_prepare_stack(
    void **stack,
    void *entry,
    void *user_arg,
    void *tcb,
    size_t *stack_size,
    size_t *guard_size,
    void **stack_base
) {
	struct {
		uint32_t prot;
		uint32_t flags;
		handle file;
		uintptr_t offset;
	} map_args = {};
	if (!*stack_size)
		*stack_size = 1024 * 1024 * 4;
	if (!(*stack)) {
		if (*guard_size == 0x1000)
			map_args.flags = (1 << 2); // VMA_FLAGS_GUARD_PAGE
		else if (*guard_size > 0)
			map_args.flags = (1 << 8); // VMA_FLAGS_RESERVE
		map_args.file = HANDLE_INVALID;
		obos_status status = OBOS_STATUS_SUCCESS;
		*stack_base = (void *)syscall5(
		    Sys_VirtualMemoryAlloc,
		    HANDLE_CURRENT,
		    NULL,
		    *stack_size + (*guard_size == 0x1000 ? 0 : *guard_size),
		    &map_args,
		    &status
		);
		if (obos_is_error(status))
			return EAGAIN;
		if (map_args.flags & (1 << 8)) {
			// Commit the stack pages, keep the guard pages reserved.
			map_args.flags = 0;
			syscall5(
			    Sys_VirtualMemoryAlloc,
			    HANDLE_CURRENT,
			    (void *)((uintptr_t)*stack_base + *guard_size),
			    *stack_size,
			    &map_args,
			    &status
			);
			if (obos_is_error(status))
				return EAGAIN;
		}
	} else
		*guard_size = 0;

	uintptr_t *top =
	    (uintptr_t *)(*stack_base + (*guard_size == 0x1000 ? 0 : *guard_size) + *stack_size);
	// Padding
	--top;
	*--top = (uintptr_t)user_arg;
	*--top = (uintptr_t)entry;
	*--top = (uintptr_t)tcb;
	*stack = top;

	return 0;
}

} // namespace mlibc

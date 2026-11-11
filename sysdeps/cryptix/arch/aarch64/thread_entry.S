#include <mlibc-asm/helpers.h>

.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
	/* Swap tls and child_tid to match the clone() argument order. */
	mov x5, x4
	mov x4, x3
	mov x3, x5

	mov x8, 56
	svc 0
	cbnz x0, .parent

	ldp x0, x1, [sp], #16

	bl __mlibc_enter_thread
	brk #0

.parent:
	ret

GNU_STACK_NOTE()

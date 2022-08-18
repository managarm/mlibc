.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
	// __mlibc_spawn_thread(flags, stack, pid_out, child_tid, tls)
	//                         x0,     x1,     x2,        x3,  x4
	//           syscall(NR_clone, flags, stack, ptid, tls, ctid)
	//                         x8,    x0,    x1,   x2,  x3,   x4

	// Swap x3 <-> x4
	mov x5, x4
	mov x4, x3
	mov x3, x5

	mov x8, 220 // NR_clone
	svc 0
	cbnz x0, .parent

	ldp x0, x1, [sp], #16

	bl __mlibc_enter_thread
	brk #0

.parent:
	ret
.section .note.GNU-stack,"",%progbits


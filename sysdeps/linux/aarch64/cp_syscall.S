.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
	mov x8, x0
	mov x0, x1
	mov x1, x2
	mov x2, x3
	mov x3, x4
	mov x4, x5
	mov x5, x6

	mrs x7, tpidr_el0
	ldr w7, [x7, #-80] // Tcb::cancelBits. See asserts in tcb.hpp.
__mlibc_syscall_begin:
	// tcbCancelEnableBit && tcbCancelTriggerBit
	mov x9, #((1 << 0) | (1 << 2))
	and x7, x7, x9
	cmp x7, x9
	b.eq cancel
	svc 0
__mlibc_syscall_end:
	ret

cancel:
	bl __mlibc_do_cancel
	brk #0
.section .note.GNU-stack,"",%progbits


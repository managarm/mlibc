.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
	// __mlibc_spawn_thread(flags, stack, pid_out, child_tid, tls)
	//                         a0,     a1,     a2,        a3,  a4
	//           syscall(NR_clone, flags, stack, ptid, tls, ctid)
	//                         a7,    a0,    a1,   a2,  a3,   a4

	// Swap a3 <-> a4
	mv a5, a4
	mv a4, a3
	mv a3, a5

	li a7, 220 // NR_clone
	ecall
	bnez a0, .parent

	ld a0, 0(sp)
	ld a1, 8(sp)
	addi sp, sp, 8
	andi sp, sp, -16
	call __mlibc_enter_thread
	unimp

.parent:
	ret
.section .note.GNU-stack,"",%progbits


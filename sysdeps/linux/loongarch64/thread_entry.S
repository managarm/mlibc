.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
	// __mlibc_spawn_thread(flags, stack, pid_out, child_tid, tls)
	//                         a0,     a1,     a2,        a3,  a4
	//           syscall(NR_clone, flags, stack, ptid, ctid, tls)
	//                         a7,    a0,    a1,   a2,  a3,   a4

	addi.d $a7, $zero, 220 // NR_clone
	syscall 0
	bnez $a0, .parent

	ld.d $a0, $sp, 0
	ld.d $a1, $sp, 8
	addi.d $sp, $sp, 8
	bstrins.d $sp, $sp, 3, 0
	b __mlibc_enter_thread
	break 0

.parent:
	ret
.section .note.GNU-stack,"",%progbits


.abiversion 1
.section ".opd","aw"
.align 8
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
	.quad .L.__mlibc_spawn_thread
	.quad .TOC.
	.quad 0

.text
.L.__mlibc_spawn_thread:
	// __mlibc_spawn_thread(flags, stack, pid_out, child_tid, tls)
	//                         x3,     x4,     x5,        x6,  x7
	//           syscall(NR_clone, flags, stack, ptid, tls, ctid)
	//                         x0,    x3,    x4,   x5,  x6,   x7

	// Swap x6<->x7
	mr 8, 7
	mr 7, 6
	mr 6, 8

	li 0, 120 // NR_clone
	sc
	nop

	cmpwi 3, 0
	bne .parent

	// load user_arg/entry from the stack
	// sysdeps/linux/generic/thread.cpp:sys_prepare_stack:55
	ld 3, 0(1)
	ld 4, 8(1)
	mr 8, 1
	addi 1, 1, 16
	
	// allocate a new stack frame for toc saving stubs
	stdu 1, -112(1)
	std 2, 40(1)

	bl __mlibc_enter_thread
	nop
	
	addi 1, 1, 112

	trap

.parent:
    blr

.section .note.GNU-stack,"",%progbits



.global __mlibcTlsdescStatic
.hidden __mlibcTlsdescStatic
.type __mlibcTlsdescStatic,@function
__mlibcTlsdescStatic:
	ldr x0, [x0, #8]
	ret

// This function depends on the Tcb layout, since it pulls out the dtv pointer
// out of the thread control block
.global __mlibcTlsdescDynamic
.hidden __mlibcTlsdescDynamic
.type __mlibcTlsdescDynamic,@function
__mlibcTlsdescDynamic:
	stp x1, x2, [sp, #-16]!
	ldr x0, [x0, #8]
	ldp x1, x2, [x0] // tlsIndex, addend
	mrs x0, tpidr_el0 // tp
	ldr x0, [x0, #-104] // tp->dtvPointers
	ldr x0, [x0, x1, lsl 3] // [tlsIndex]
	add x0, x0, x2 // + addend
	mrs x1, tpidr_el0 // tp
	sub x0, x0, x1 // result - tp
	ldp x1, x2, [sp], #16
	ret

.global pltRelocateStub
pltRelocateStub:
	// we need to save / restore all registers than can hold function arguments
	// we do not need to save callee-saved registers as they will not be trashed by lazyRelocate
	// TODO: save floating point argument registers

	stp x0, x1, [sp, #-16]!

	// pointer to PLT entry
	ldr x1, [sp, #24]
	ldr x0, [x16]
	sub x1, x1, x0
	asr x0, x0, #3

	// pointer GOT
	sub x0, x16, #8 // &PLTGOT[1]

	stp x2, x3, [sp, #-16]!
	stp x4, x5, [sp, #-16]!
	stp x6, x7, [sp, #-16]!
	stp x8, x30, [sp, #-16]!

	bl lazyRelocate
	mov x9, x0

	ldp x8, x30, [sp], #16
	ldp x6, x7, [sp], #16
	ldp x4, x5, [sp], #16
	ldp x2, x1, [sp], #16

	ldp x0, x1, [sp], #16
	add sp, sp, #16
	br x9

.section .note.GNU-stack,"",%progbits


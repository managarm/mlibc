
.global __mlibcTlsdescStatic
.hidden __mlibcTlsdescStatic
.type __mlibcTlsdescStatic,@function
__mlibcTlsdescStatic:
	ldr x0, [x0, #8]
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


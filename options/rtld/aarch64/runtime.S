
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
	ldr x1, [x0, #8] // TlsdescData *
	ldp x0, x2, [x1] // tlsIndex, addend

	mrs x1, tpidr_el0
	ldr x1, [x1, #-112] // dtvSize
	cmp x1, x0
	bls .Lslowpath

	mrs x1, tpidr_el0
	ldr x1, [x1, #-104] // dtvPointers
	ldr x1, [x1, x0, lsl 3] // dtv entry
	cbz x1, .Lslowpath

	// success
	add x0, x1, x2 // dtv entry + addend
	mrs x1, tpidr_el0
	sub x0, x0, x1 // - tp
	ldp x1, x2, [sp], #16
	ret

.Lslowpath:
	ldp x1, x2, [sp], #16

	// Preserve all registers except x0.
	stp x29, x30, [sp, #-16]!
	mov x29, sp

	// Save all integer registers x1-x28 (x29, x30 already saved)
	stp x1, x2, [sp, #-16]!
	stp x3, x4, [sp, #-16]!
	stp x5, x6, [sp, #-16]!
	stp x7, x8, [sp, #-16]!
	stp x9, x10, [sp, #-16]!
	stp x11, x12, [sp, #-16]!
	stp x13, x14, [sp, #-16]!
	stp x15, x16, [sp, #-16]!
	stp x17, x18, [sp, #-16]!
	stp x19, x20, [sp, #-16]!
	stp x21, x22, [sp, #-16]!
	stp x23, x24, [sp, #-16]!
	stp x25, x26, [sp, #-16]!
	stp x27, x28, [sp, #-16]!

	// arg0 = TlsdescData*
	ldr x0, [x0, #8]
	bl __mlibc_tlsdesc_slowpath

	ldp x27, x28, [sp], #16
	ldp x25, x26, [sp], #16
	ldp x23, x24, [sp], #16
	ldp x21, x22, [sp], #16
	ldp x19, x20, [sp], #16
	ldp x17, x18, [sp], #16
	ldp x15, x16, [sp], #16
	ldp x13, x14, [sp], #16
	ldp x11, x12, [sp], #16
	ldp x9, x10, [sp], #16
	ldp x7, x8, [sp], #16
	ldp x5, x6, [sp], #16
	ldp x3, x4, [sp], #16
	ldp x1, x2, [sp], #16

	ldp x29, x30, [sp], #16
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


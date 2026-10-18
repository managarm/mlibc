
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

	mrs x1, tpidr_el0 // tp
	sub x1, x1, #96 // address of tp->dtvPointers
	ldar x1, [x1] // load dtvPointers with acquire
	ldr x0, [x1, x0, lsl 3] // dtvPointers[tlsIndex]

	add x0, x0, x2 // + addend
	mrs x1, tpidr_el0 // tp
	sub x0, x0, x1 // result - tp
	ldp x1, x2, [sp], #16
	ret

.section .note.GNU-stack,"",%progbits

.abiversion 1
.global pltRelocateStub
pltRelocateStub:
	// we need to save / restore all registers than can hold function arguments
	// we do not need to save callee-saved registers as they will not be trashed by lazyRelocate
    // TODO: save floating point argument registers
    // todo(localcc): implement

    stdu 1, -112(1)
    std 2, 40(1)

    std 3, 48(1)
    std 4, 56(1)

    ld 3, 112(1)
    ld 4, 120(1)

    bl lazyRelocate
    nop
    mtctr 3

    ld 3, 48(1)
    ld 4, 56(1)

    addi 1, 1, 112

    bctr

.section .note.GNU-stack,"",%progbits
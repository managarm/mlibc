.abiversion 1
.global pltRelocateStub
pltRelocateStub:
	# we need to save / restore all registers than can hold function arguments
	# we do not need to save callee-saved registers as they will not be trashed by lazyRelocate
    # TODO: save floating point argument registers
    // todo(localcc): implement
    trap
.section .note.GNU-stack,"",%progbits
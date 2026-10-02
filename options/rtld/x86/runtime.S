.global __mlibcTlsdescStatic
.hidden __mlibcTlsdescStatic
.type __mlibcTlsdescStatic, @function
__mlibcTlsdescStatic:
	movl 4(%eax), %eax
	ret

.global __mlibcTlsdescDynamic
.hidden __mlibcTlsdescDynamic
.type __mlibcTlsdescDynamic, @function
__mlibcTlsdescDynamic:
	movl 4(%eax), %eax # TlsdescData*
	movl (%eax), %ecx # tlsIndex
	movl 4(%eax), %edx # addend

	movl %gs:8, %eax # dtvPointers
	movl (%eax,%ecx,4), %eax # dtvPointers[tlsIndex]

	add %edx, %eax
	sub %gs:0, %eax # - TP
	ret

.global pltRelocateStub
# save / restore all registers that can hold function parameters
pltRelocateStub:
	# we need to save / restore all registers than can hold function arguments
	# we do not need to save callee-saved registers as they will not be trashed by lazyRelocate
	# TODO: save floating point argument registers
	ud2

.section .note.GNU-stack,"",%progbits

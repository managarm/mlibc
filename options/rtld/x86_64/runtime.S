.global __mlibcTlsdescStatic
.hidden __mlibcTlsdescStatic
.type __mlibcTlsdescStatic, @function
__mlibcTlsdescStatic:
	mov 8(%rax), %rax
	ret

.global __mlibcTlsdescDynamic
.hidden __mlibcTlsdescDynamic
.type __mlibcTlsdescDynamic, @function
__mlibcTlsdescDynamic:
	push %rbx
	push %rcx

	mov 8(%rax), %rax

	mov (%rax), %rbx // index
	mov 8(%rax), %rcx // addend

	mov %fs:16, %rax // *tp->dtvPointers
	mov (%rax, %rbx, 8), %rax // dtvPointers[0][index]
	add %rcx, %rax // + addend
	sub %fs:0, %rax

	pop %rcx
	pop %rbx
	ret

.global pltRelocateStub
pltRelocateStub:
	# we need to save / restore all registers than can hold function arguments
	# we do not need to save callee-saved registers as they will not be trashed by lazyRelocate
	# TODO: save floating point argument registers

	push %rsi
	push %rdi
	mov 16(%rsp), %rdi
	mov 24(%rsp), %rsi

	push %rax
	push %rcx
	push %rdx
	push %r8
	push %r9
	push %r10

	call lazyRelocate
	mov %rax, %r11

	pop %r10
	pop %r9
	pop %r8
	pop %rdx
	pop %rcx
	pop %rax
	
	pop %rdi
	pop %rsi
	add $16, %rsp
	jmp *%r11

.section .note.GNU-stack,"",%progbits


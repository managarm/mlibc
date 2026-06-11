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
	# Save registers we use in the fast path
	push %rbx
	push %rcx

	mov 8(%rax), %rbx # TlsdescData*
	mov (%rbx), %rcx # tlsIndex

	cmp %fs:8, %rcx # dtvSize
	jae .Lslowpath

	mov %fs:16, %rbx # dtvPointers
	mov (%rbx, %rcx, 8), %rcx # dtvPointers[tlsIndex]
	test %rcx, %rcx
	jz .Lslowpath

	# Fast path success: rcx = dtv entry
	mov 8(%rax), %rbx # TlsdescData*
	mov 8(%rbx), %rax # addend
	add %rcx, %rax
	sub %fs:0, %rax

	pop %rcx
	pop %rbx
	ret

.Lslowpath:
	pop %rcx
	pop %rbx

	# Preserve all registers except rax and flags
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
	sub $8, %rsp

	mov 8(%rax), %rdi # arg0 = TlsdescData*
	call __mlibc_tlsdesc_slowpath
	add $8, %rsp

	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
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


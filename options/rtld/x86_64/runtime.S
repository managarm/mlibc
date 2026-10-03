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

	mov 8(%rax), %rbx # TlsdescData*
	mov (%rbx), %rcx # tlsIndex

	mov %fs:16, %rax # dtvPointers
	mov (%rax, %rcx, 8), %rax # dtvPointers[tlsIndex]

	mov 8(%rbx), %rcx # addend
	add %rcx, %rax
	sub %fs:0, %rax

	pop %rcx
	pop %rbx
	ret

.section .note.GNU-stack,"",%progbits

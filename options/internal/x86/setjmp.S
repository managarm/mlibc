
.type __setjmp, "function"
__setjmp:
	mov 4(%esp), %eax       # Save argument (buffer) in edi
	mov %ebx, 0x00(%eax)
	mov %ebp, 0x04(%eax)
	mov %esi, 0x08(%eax)
	mov %edi, 0x0c(%eax)

	lea 4(%esp), %ecx       # esp before return eip is pushed
	mov %ecx, 0x10(%eax)
	mov (%esp), %ecx        # Return eip
	mov %ecx, 0x14(%eax)

	test %edx, %edx
	jnz 1f
	xor %eax, %eax
	ret

1:
	jmp __sigsetjmp@PLT

.global setjmp
.type setjmp, "function"
.global _setjmp
.type _setjmp, "function"
setjmp:
_setjmp:
	xor %edx, %edx
	jmp __setjmp

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	mov $1, %edx
	jmp __setjmp

.global longjmp
.type longjmp, "function"
.global _longjmp
.type _longjmp, "function"
longjmp:
_longjmp:
	mov 4(%esp), %ecx
	mov 0x00(%ecx), %ebx
	mov 0x04(%ecx), %ebp
	mov 0x08(%ecx), %esi
	mov 0x0c(%ecx), %edi

	mov 8(%esp), %eax
	test %eax, %eax
	jnz 1f
	inc %eax
1:
	mov 0x10(%ecx), %esp
	jmp *0x14(%ecx)

.section .note.GNU-stack,"",%progbits


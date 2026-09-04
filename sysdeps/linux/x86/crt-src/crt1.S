.section .text
.global _start

.type _start, %function
.type main, %function
.type __mlibc_entry, %function

.cfi_startproc
_start:
.cfi_undefined eip
	xor %ebp, %ebp
	mov %esp, %ecx

	/* Align the stack to 16-byte boundary per i386 psABI */
	lea -8(%esp), %esp

	push $main
	push %ecx
	call __mlibc_entry
.cfi_endproc

.section .note.GNU-stack,"",%progbits

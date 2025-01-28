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
	push $main
	push %ecx
	call __mlibc_entry
.cfi_endproc

.section .note.GNU-stack,"",%progbits

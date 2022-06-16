.section .text
.global _start
_start:
	mv a0, sp
	la a1, main
	call __mlibc_entry

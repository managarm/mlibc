.extern __mlibc_entry@plt

.section .text
.global _start
_start:
	move $a0, $sp
	la $a1, main
	bl %plt(__mlibc_entry)
	break 0

.section .note.GNU-stack,"",%progbits

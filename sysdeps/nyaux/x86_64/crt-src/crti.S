.section .init
.global _init
_init:
	push %rax

.section .fini
.global _fini
_fini:
	push %rax
.section .note.GNU-stack,"",%progbits


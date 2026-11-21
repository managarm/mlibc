.section .init
.global _init
_init:
	pushl %eax

.section .fini
.global _fini
_fini:
	pushl %eax

.section .note.GNU-stack,"",%progbits

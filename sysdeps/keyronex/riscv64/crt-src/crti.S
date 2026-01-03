.section .init
.global _init
_init:
	ret

.section .fini
.global _fini
_fini:
	ret
.section .note.GNU-stack,"",%progbits

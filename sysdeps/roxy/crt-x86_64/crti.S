.section .init
.global _init
.type _init, @function
_init:
	push %rax

.section .fini
.global _fini
.type _fini, @function
_fini:
	push %rax

.section .note.GNU-stack,"",@progbits


.section .data
.hidden __dso_handle
.global __dso_handle
__dso_handle:
	.quad __dso_handle

.section .init
.hidden _init
.global _init
_init:

.section .fini
.hidden _fini
.global _fini
_fini:

.section .ctors
.hidden __CTOR_LIST__
.global __CTOR_LIST__
__CTOR_LIST__:

.section .dtors
.hidden __DTOR_LIST__
.global __DTOR_LIST__
__DTOR_LIST__:

.section .note.GNU-stack,"",%progbits


.section .ctors,"w"
.global __CTOR_LIST__
__CTOR_LIST__:

.section .dtors,"w"
.global __DTOR_LIST__
__DTOR_LIST__:

.section .data
.global __dso_handle
__dso_handle:
	.quad 0

.section .ctors,"w"
.global __CTOR_END__
__CTOR_END__:
	.quad 0

.section .dtors,"w"
.global __DTOR_END__
__DTOR_END__:
	.quad 0

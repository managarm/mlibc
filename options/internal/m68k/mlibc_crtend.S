
.hidden __mlibc_do_ctors
.hidden __mlibc_do_dtors

.section .init
	bsr.l __mlibc_do_ctors
	rts

.section .fini
	bsr.l __mlibc_do_dtors
	rts

.section .ctors
.hidden __CTOR_END__
.global __CTOR_END__
__CTOR_END__:

.section .dtors
.hidden __DTOR_END__
.global __DTOR_END__
__DTOR_END__:

.section .note.GNU-stack,"",%progbits

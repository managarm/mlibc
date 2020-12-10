// TODO: implement this

.extern __mlibc_log_setjmp_error

.global setjmp
.type setjmp, "function"
setjmp:
	stp x29, x30, [sp, #-16]!
	bl __mlibc_log_setjmp_error
	ldp x29, x30, [sp], #16
	ret

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	stp x29, x30, [sp, #-16]!
	bl __mlibc_log_setjmp_error
	ldp x29, x30, [sp], #16
	ret

.global longjmp
.type longjmp, "function"
longjmp:
	stp x29, x30, [sp, #-16]!
	bl __mlibc_log_setjmp_error
	ldp x29, x30, [sp], #16
	ret


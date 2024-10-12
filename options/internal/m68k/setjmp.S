.type __setjmp, "function"
__setjmp:
	illegal

.global setjmp
.type setjmp, "function"
setjmp:
	illegal

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	illegal

.global longjmp
.type longjmp, "function"
longjmp:
	illegal

.section .note.GNU-stack,"",%progbits

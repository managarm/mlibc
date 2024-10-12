.global feclearexcept
.type feclearexcept,@function
feclearexcept:
	illegal

.global feraiseexcept
.type feraiseexcept,@function
feraiseexcept:
	illegal

.global __fesetround
.hidden __fesetround
.type __fesetround,@function
__fesetround:
	illegal

.global fegetround
.type fegetround,@function
fegetround:
	illegal

.global fegetenv
.type fegetenv,@function
fegetenv:
	illegal

.global fesetenv
.type fesetenv,@function
fesetenv:
	illegal

.global fetestexcept
.type fetestexcept,@function
fetestexcept:
	illegal

.section .note.GNU-stack,"",%progbits

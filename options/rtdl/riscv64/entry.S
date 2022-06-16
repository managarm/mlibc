.global _start
_start:
	call relocateSelf

	mv a0, sp
	call interpreterMain

	jr a0


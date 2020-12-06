
.global _start
_start:
	bl relocateSelf

	mov x0, sp
	bl interpreterMain

	br x0

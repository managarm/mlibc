.global _start
_start:
	pcaddu18i $ra, %call36(relocateSelf)
	jirl $ra, $ra, 0

	move $fp, $zero
	move $a0, $sp
	pcaddu18i $ra, %call36(interpreterMain)
	jirl $ra, $ra, 0

	jr $a0

.section .note.GNU-stack,"",%progbits


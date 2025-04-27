.global feclearexcept
.type feclearexcept, %function
feclearexcept:
	li.w $t0, 0x1f0000
	and $a0, $a0, $t0
	movfcsr2gr $t1, $fcsr0
	andn $t1, $t1, $a0
	movgr2fcsr $fcsr0, $t1
	li.w $a0, 0
	jr $ra

.global feraiseexcept
.type feraiseexcept, %function
feraiseexcept:
	li.w $t0, 0x1f0000
	and $a0, $a0, $t0
	movfcsr2gr $t1, $fcsr0
	or $t1, $t1, $a0
	movgr2fcsr $fcsr0, $t1
	li.w $a0, 0
	jr $ra

.global fetestexcept
.type fetestexcept, %function
fetestexcept:
	li.w $t0, 0x1f0000
	and $a0, $a0, $t0
	movfcsr2gr $t1, $fcsr0
	and $a0, $t1, $a0
	jr $ra

.global fegetround
.type fegetround, %function
fegetround:
	movfcsr2gr $t0, $fcsr0
	andi $a0, $t0, 0x300
	jr $ra

.global __fesetround
.type __fesetround, %function
__fesetround:
	li.w $t0, 0x300
	and $a0, $a0, $t0
	movfcsr2gr $t1, $fcsr0
	andn $t1, $t1, $t0
	or $t1, $t1, $a0
	movgr2fcsr $fcsr0, $t1
	li.w $a0, 0
	jr $ra

.global fegetenv
.type fegetenv, %function
fegetenv:
	movfcsr2gr $t0, $fcsr0
	st.w $t0, $a0, 0
	li.w $a0, 0
	jr $ra

.global fesetenv
.type fesetenv, %function
fesetenv:
	addi.d $t0, $a0, 1
	beq $t0, $r0, 1f
	ld.w $t0, $a0, 0
1:	movgr2fcsr $fcsr0, $t0
	li.w $a0, 0
	jr $ra
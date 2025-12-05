#include "mlibc-asm/helpers.h"

.hidden __setjmp
.type __setjmp, "function"
__setjmp:
	st.d $ra, $a0, 0
	st.d $sp, $a0, 8
	st.d $r21, $a0, 16
	st.d $fp, $a0, 24
	st.d $s0, $a0, 32
	st.d $s1, $a0, 40
	st.d $s2, $a0, 48
	st.d $s3, $a0, 56
	st.d $s4, $a0, 64
	st.d $s5, $a0, 72
	st.d $s6, $a0, 80
	st.d $s7, $a0, 88
	st.d $s8, $a0, 96
	fst.d $fs0, $a0, 104
	fst.d $fs1, $a0, 112
	fst.d $fs2, $a0, 120
	fst.d $fs3, $a0, 128
	fst.d $fs4, $a0, 136
	fst.d $fs5, $a0, 144
	fst.d $fs6, $a0, 152
	fst.d $fs7, $a0, 160
	bnez $t1, 1f
	move $a0, $zero
	ret

1:
	b __sigsetjmp

.global setjmp
.type setjmp, "function"
.global _setjmp
.type _setjmp, "function"
setjmp:
_setjmp:
	move $t1, $zero
	b __setjmp

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	addi.w $t1, $zero, 1
	b __setjmp

.global longjmp
.type longjmp, "function"
.global _longjmp
.type _longjmp, "function"
longjmp:
_longjmp:
	ld.d $ra, $a0, 0
	ld.d $sp, $a0, 8
	ld.d $r21, $a0, 16
	ld.d $fp, $a0, 24
	ld.d $s0, $a0, 32
	ld.d $s1, $a0, 40
	ld.d $s2, $a0, 48
	ld.d $s3, $a0, 56
	ld.d $s4, $a0, 64
	ld.d $s5, $a0, 72
	ld.d $s6, $a0, 80
	ld.d $s7, $a0, 88
	ld.d $s8, $a0, 96
	fld.d $fs0, $a0, 104
	fld.d $fs1, $a0, 112
	fld.d $fs2, $a0, 120
	fld.d $fs3, $a0, 128
	fld.d $fs4, $a0, 136
	fld.d $fs5, $a0, 144
	fld.d $fs6, $a0, 152
	fld.d $fs7, $a0, 160
	sltui   $a0, $a1, 1
	add.d   $a0, $a0, $a1
	jr      $ra

GNU_STACK_NOTE()


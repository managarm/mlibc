#include <bits/ansi/fenv.h>

.global feclearexcept
.type feclearexcept,@function
feclearexcept:
	move.l 4(%sp), %d0
	andi.l #~FE_ALL_EXCEPT, %d0
	bne 1f

	fmove.l %fpsr, %d1
	movel 4(%sp), %d0
	not.l %d0
	and.l %d0, %d1
	fmove.l %d1, %fpsr

	moveq.l #0, %d0
	rts

1:
	moveq.l #-1, %d0
	rts

.global feraiseexcept
.type feraiseexcept,@function
feraiseexcept:
	move.l 4(%sp), %d0
	andi.l #~FE_ALL_EXCEPT, %d0
	bne 1f

	fmove.l %fpsr, %d1
	or.l 4(%sp), %d1
	fmove.l %d1, %fpsr

	moveq.l #0, %d0
	rts

1:
	moveq.l #-1, %d0
	rts

.global __fesetround
.hidden __fesetround
.type __fesetround,@function
__fesetround:
	fmove.l %fpcr, %d1
	andi.l #~FE_UPWARD, %d1
	or.l 4(%sp), %d1
	fmove.l %d1, %fpcr
	moveq.l #0, %d0
	rts

.global fegetround
.type fegetround,@function
fegetround:
	fmove.l %fpcr, %d0
	andi.l #FE_UPWARD, %d0
	rts

.global fegetenv
.type fegetenv,@function
fegetenv:
	move.l 4(%sp), %a0

	fmove.l %fpcr, %d0
	move.l %d0, (%a0)

	fmove.l %fpsr, %d0
	move.l %d0, 4(%a0)

	fmove.l %fpiar, %d0
	move.l %d0, 8(%a0)

	moveq.l #0, %d0
	rts

.global fesetenv
.type fesetenv,@function
fesetenv:
	move.l 4(%sp), %a0

	cmp.l #-1, %a0
	beq 1f

	move.l (%a0), %d0
	fmove.l %d0, %fpcr

	move.l 4(%a0), %d0
	fmove.l %d0, %fpsr

	move.l 8(%a0), %d0
	fmove.l %d0, %fpiar

	moveq.l #0, %d0
	rts

1:
	clr.l %d0
	fmove.l %d0, %fpcr
	fmove.l %d0, %fpsr
	fmove.l %d0, %fpiar
	moveq.l #0, %d0
	rts

.global fetestexcept
.type fetestexcept,@function
fetestexcept:
	fmove.l %fpsr, %d0
	and.l 4(%sp), %d0
	rts

.section .note.GNU-stack,"",%progbits

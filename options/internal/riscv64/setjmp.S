.global setjmp
.type setjmp, "function"
.global _setjmp
.type _setjmp, "function"
setjmp:
_setjmp:
	sd ra, 0(a0)
	sd s0, 8(a0)
	sd s1, 16(a0)
	sd s2, 24(a0)
	sd s3, 32(a0)
	sd s4, 40(a0)
	sd s5, 48(a0)
	sd s6, 56(a0)
	sd s7, 64(a0)
	sd s8, 72(a0)
	sd s9, 80(a0)
	sd s10, 88(a0)
	sd s11, 96(a0)
	sd sp, 104(a0)
	fsd fs0, 112(a0)
	fsd fs1, 120(a0)
	fsd fs2, 128(a0)
	fsd fs3, 136(a0)
	fsd fs4, 144(a0)
	fsd fs5, 152(a0)
	fsd fs6, 160(a0)
	fsd fs7, 168(a0)
	fsd fs8, 176(a0)
	fsd fs9, 184(a0)
	fsd fs10, 192(a0)
	fsd fs11, 200(a0)
	li a0, 0
	ret

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	unimp // TODO

.global longjmp
.type longjmp, "function"
.global _longjmp
.type _longjmp, "function"
longjmp:
_longjmp:
	ld ra,0(a0)
	ld s0,8(a0)
	ld s1,16(a0)
	ld s2,24(a0)
	ld s3,32(a0)
	ld s4,40(a0)
	ld s5,48(a0)
	ld s6,56(a0)
	ld s7,64(a0)
	ld s8,72(a0)
	ld s9,80(a0)
	ld s10,88(a0)
	ld s11,96(a0)
	ld sp,104(a0)
	fld fs0,112(a0)
	fld fs1,120(a0)
	fld fs2,128(a0)
	fld fs3,136(a0)
	fld fs4,144(a0)
	fld fs5,152(a0)
	fld fs6,160(a0)
	fld fs7,168(a0)
	fld fs8,176(a0)
	fld fs9,184(a0)
	fld fs10,192(a0)
	fld fs11,200(a0)
	seqz a0,a1
	add a0,a0,a1
	ret
.section .note.GNU-stack,"",%progbits

